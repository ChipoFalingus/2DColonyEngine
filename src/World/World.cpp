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


//void addPaths() {
//	int mapDim = calculateMapSize();
//    auto points = createVoronoiMap(10, -mapDim, mapDim, -mapDim, mapDim);
//
//    for (int i = 0; i < points.size(); i++) {
//        auto& point1 = points[getRandomInt(0, points.size() - 1)];
//	    auto& point2 = points[getRandomInt(0, points.size() - 1)];
//
//        auto line = findPath(point1.first, point1.second, { point2.first, point2.second });
//
//        for (auto& p : line) {
//            if (!getTileRef(p.first, p.second).containsItem("Path")) {
//                getTileRef(p.first, p.second).addObject("Path");
//            }
//		}
//	}
//}



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

    for (int i = 0; i < 5000; i++) {
        entt::entity member = registry.create();

        int spawnX = x + getRandomInt(-10, 10);
        int spawnY = y + getRandomInt(-10, 10);
        registry.emplace<Position>(member, spawnX, spawnY);

        registry.emplace<Movable>(member, 0.2f, 0.2f, 0.0f, spawnX, spawnY);

        registry.emplace<SquadMemberComponent>(member, squadEntity);

        registry.emplace<Renderable>(member, L'Z', glm::vec3(0.0f, 1.0f, 0.0f));
        registry.emplace<Name>(member, "Zombie");

        controller.members.push_back(member);
    }
}

// Adds creatures to the world
void World::addCreatures() {
    int range = 3;
    for (int i = 0; i < 0; i++) {
        int x = getRandomInt(-range, range);
        int y = getRandomInt(-range, range);
        auto v = spawnVillager(x, y);
        objectManager.addObject(x, y, v);
    }

    for (int i = 0; i < 1; i++) {
        spawnSquad(getRandomInt(-20, 20), getRandomInt(-20, 20));
    }
    

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

        auto newMap = Game::getInstance()
            .getLightManager()
            .BFSLight();

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

    for (auto& chunkPair : chunks) {

        std::unordered_map<tileType, int> typeTracker;
        std::unordered_map<std::string, int> displayTracker;

        for (int j = 0; j < chunkDim; j++) {
            for (int k = 0; k < chunkDim; k++) {
                Tile& tile = chunkPair.second.tiles[j][k];
                typeTracker[tile.type]++;

                /*if (tile.hasItems) {
                    const auto& tileObjects = objectManager.getObjectsAt(chunkPair.first.first * chunkDim + j, chunkPair.first.second * chunkDim + k);
                    if (!tileObjects.empty() && tileObjects[0]) {
                        std::string display = tileObjects[0]->name;
                        displayTracker[display]++;
                    }
                }*/

                /*if (tile.containsItem("Outpost")) {
                    displayTracker["Outpost"] = 999;
                }*/
            }
        }

        tileType type = tileType::GRASS;
        int maxCount = 0;

        for (auto& pair : typeTracker) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                type = pair.first;
            }
        }

        
        std::string dominantItem = "";
        int maxItemCount = 0;

        for (auto& pair : displayTracker) {
            if (pair.second > maxItemCount) {
                maxItemCount = pair.second;
                dominantItem = pair.first;
            }
        }

        int itemThreshold = 16;

        chunkPair.second.dominantDisplay = getTileDisplay(type);

        /*if (maxItemCount > itemThreshold) {
			auto item = ObjectRegistry::getInstance().get(dominantItem);
			auto display = VisualRegistry::getInstance().get(dominantItem);
            chunkPair.second.dominantDisplay = { {display.displayChar}, {display.displayColor} };
        }
        else {
            chunkPair.second.dominantDisplay = getTileDisplay(type);
        }*/
    }
}