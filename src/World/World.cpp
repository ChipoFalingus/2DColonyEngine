#include <cmath>
#include <unordered_map>
#include <algorithm>

#include "Utility/ThreadPool.h"
#include "World.h"
#include "Chunk.h"
#include "Game.h"
#include "Entities/Squad.h"
#include "Entities/CreatureComponents.h"

World& World::get() {
    static World instance;
    return instance;
}

World& mainWorld = World::get();

void World::renderWorld() {
    int mapDim = calculateMapSize();

    // Renders all chunks
    int chunkRadius = (mapDim / chunkDim);


    for (int cy = -chunkRadius; cy < chunkRadius; cy++) {
        for (int cx = -chunkRadius; cx < chunkRadius; cx++) {
            loadOrGenerateChunk(cx, cy);
        }
    }

    for (int y = -mapDim; y < mapDim; y++) {
        for (int x = -mapDim; x < mapDim; x++) {
            Tile& tile = getTileRef(x, y);
            tile.getTile(x, y);
        }
    }
}

void createRegions() {
    int size = calculateMapSize();
    int regionID = 0;

    for (int x = -size; x < size; x++) {
        for (int y = -size; y < size; y++) {
            Tile& tile = getTileRef(x, y);
            if (!tile.walkable || tile.region != -1)
                continue;

            std::queue<std::pair<int, int>> q;
            q.push({ x, y });
            tile.region = regionID;

            std::vector<std::pair<int, int>> directions = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            };

            while (!q.empty()) {
                auto [cx, cy] = q.front();
                q.pop();

                for (auto [dx, dy] : directions) {
                    int nx = cx + dx;
                    int ny = cy + dy;

                    Tile& nt = getTileRef(nx, ny);

                    if (!nt.walkable) continue;
                    if (nt.region != -1) continue;

                    nt.region = regionID;
                    q.push({ nx, ny });
                }
            }

            regionID++;
        }
    }
}

void spawnSquad(int x, int y) {
    auto& registry = mainWorld.registry;
    entt::entity squadEntity = registry.create();
    auto& controller = registry.emplace<SquadController>(squadEntity);

    controller.groupTargetPos = { x, y };
    controller.state = SquadState::IDLE;

    for (int i = 0; i < 20; i++) {
        entt::entity member = registry.create();

        int spawnX = getRandomInt(x-10, x+10);
        int spawnY = getRandomInt(y-10, y+10);

        if (!getTileRef(spawnX, spawnY).walkable) {
            i--;
            continue;
		}

        registry.emplace<Position>(member, spawnX, spawnY);
        registry.emplace<Renderable>(member, L'Z', glm::vec3(1.0f, 1.0f, 1.0f));

        registry.emplace<Movable>(member);

        registry.emplace<SquadMemberComponent>(member, squadEntity);

        registry.emplace<Name>(member, "Zombie");
        registry.emplace<Hostile>(member);
        registry.emplace<Health>(member, 40);

        registry.emplace<Zombie>(member);
        mainWorld.objectManager.addObject(spawnX, spawnY, member);

        controller.members.push_back(member);
    }
}

// Adds creatures to the world
void World::addCreatures() {
    int range = 20;
    for (int i = 0; i < 3; i++) {
        int x = getRandomInt(-range, range);
        int y = getRandomInt(-range, range);
        auto v = spawnVillager(x, y);
        objectManager.addObject(x, y, v);
    }

    for (int i = 0; i < 0; i++) {
        int size = calculateMapSize();
		int x = getRandomInt(-size, size);
		int y = getRandomInt(-size, size);
        if (getTileRef(x,y).walkable)
            spawnSquad(x, y);
    }

    auto entity = registry.create();
    registry.emplace<Name>(entity, "Cat");
    registry.emplace<Position>(entity, 0, 0);
    registry.emplace<Renderable>(entity, L'c', glm::vec3(0.6f, 0.6f, 0.6f));

    registry.emplace<Movable>(entity, .1f, .1f, 0.0f, 0, 0, false);
    registry.emplace<Health>(entity, 100);
    registry.emplace<HungerNeed>(entity, 100);
    registry.get<HungerNeed>(entity).weight = .0f;
    registry.emplace<TiredNeed>(entity, 0);

    mainWorld.objectManager.addObject(0, 0, entity);

    registry.emplace<JobComponent>(entity, nullptr);
}

std::vector<entt::entity> World::getAllVillagers() {
    std::vector<entt::entity> result;

	auto view = registry.view<Movable>();

    for (auto entity : view) {
        if (registry.any_of<Movable>(entity)) {
            result.push_back(entity);
        }
    }
    return result;
}

// Central method to generate the world
void World::generateWorld() {
    // World setup

    // Another thread for world gen, main thread does loading screen
    std::thread([this] {
        currentlyRendering = true;
        initializePermutation(seed);
        setSeaLevel(0.0f);
        createMapIslands();

        renderWorld();
        addCreatures();

        updateMiniMap();

        initLightMap();
        initTemperatureMap();

        auto newMap = Game::getInstance().getLightManager().BFSLight();

        setLightMap(newMap);

        createRegions();

        //addPaths();

        Game::getInstance().getUIManager().swapFrame(UI::Loading, UI::InGame);

        currentlyRendering = false;
        rendered = true;

        }).detach();

    
}

Chunk& World::loadOrGenerateChunk(int x, int y) {
    std::pair<int, int> pair = std::make_pair(x, y);

    auto it = chunks.find(pair);
    if (it == chunks.end()) {
        Chunk newChunk;
        newChunk.chunkX = x;
        newChunk.chunkY = y;

        float avgHeight = 0.0f;

        int baseX = x * chunkDim;
        int baseY = y * chunkDim;

        for (int i = 0; i < chunkDim; i++) {
            for (int j = 0; j < chunkDim; j++) {
                int worldX = baseX + i;
                int worldY = baseY + j;

                newChunk.tiles[i][j] = assignTileTypes(worldX, worldY);
                avgHeight += newChunk.tiles[i][j].altitude;
                
            }
        }

        //std::cout << "Chunk Rendered at " << newChunk.chunkX * chunkDim << " " << newChunk.chunkY * chunkDim << std::endl;

		chunksRendered++;
        newChunk.avgHeight = avgHeight / (chunkDim * chunkDim);

        addTileToMinimap(newChunk);
        auto insertResult = chunks.emplace(std::make_pair(pair, std::move(newChunk)));
        
        return insertResult.first->second;
    }
    else {
        return it->second;
    }
}


Chunk* World::getChunk(int x, int y) {
    std::pair<int, int> pair = { x, y };
    auto it = chunks.find(pair);
    if (it != chunks.end()) {
        return &it->second;
    }
    else {
        return nullptr;
    }
}

void World::addTileToMinimap(Chunk& chunk) {
    std::map<tileType, int> typeTracker;

    for (int j = 0; j < chunkDim; j++) {
        for (int k = 0; k < chunkDim; k++) {
            Tile& tile = chunk.tiles[j][k];
            typeTracker[tile.type]++;
        }
    }


    tileType type{};
    int maxCount = 0;

    for (auto& i : typeTracker) {
        if (i.second > maxCount) {
            maxCount = i.second;
            type = i.first;
        }
    }

    tileDisplay display = getTileDisplay(type);

	chunk.dominantDisplay = display;
}

// Beefy method that is super slow
void World::updateMiniMap() {

    for (auto& [coordinate, chunk] : chunks) {

        std::map<tileType, int> typeTracker;
        std::map<std::string, int> displayTracker;

        for (int j = 0; j < chunkDim; j++) {
            for (int k = 0; k < chunkDim; k++) {
                Tile& tile = chunk.tiles[j][k];
                typeTracker[tile.type]++;

                const auto& tileObjects = objectManager.getObjectsAt(coordinate.first * chunkDim + j, coordinate.second * chunkDim + k);
                if (!tileObjects.empty()) {
					displayTracker[registry.get<Name>(tileObjects[0]).name]++;
                }
            }
        }

        std::string dominantItem = "";
        int maxItemCount = 0;

        for (const auto& [string, count] : displayTracker) {
            if (count > maxItemCount) {
                maxItemCount = count;
                dominantItem = string;
            }
        }

        int itemThreshold = 16;

        if (maxItemCount > itemThreshold) {
			auto display = ObjectRegistry::getInstance().getStaticComponent<Renderable>(dominantItem);
            if (display) {
                chunk.dominantDisplay = { {display->character}, {display->color} };
            }
            else {
                chunk.dominantDisplay = { '?', glm::vec3(1.0f, 0.0f, 0.0f)};
            }
        }
        else {
            tileType dominantTileType{};
            int maxTileCount = -1;

            for (const auto& [tType, count] : typeTracker) {
                if (count > maxTileCount) {
                    maxTileCount = count;
                    dominantTileType = tType;
                }
            }

            chunk.dominantDisplay = getTileDisplay(dominantTileType);
        }
    }
}