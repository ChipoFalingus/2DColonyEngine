#include <iostream>
#include <SFML/Graphics.hpp>
#include <queue>
#include <unordered_set>

#include "Utility/CreatureUtils.h"
#include "Utility/mathUtils.h"
#include "Utility/Vec2.h"
#include "Tile.h"
#include "Chunk.h"
#include "Utility/Light.h"
#include "Utility/Globals.h"
#include "Game.h"
#include "Entities/ItemComponents.h"
#include "Entities/CreatureComponents.h"
#include <set>

// A lot of this stuff is world generation, so it should be moved to a separate file later
// A real big mess this all is :(
// ^ A few months later, a less big mess this all is :)

std::string typeToString(tileType type) {
    switch (type) {
    case(tileType::GRASS): {
        return "Grass";
    }case(tileType::SOIL): {
		return "Soil";
    }case(tileType::SAND): {
        return "Sand";
    }case(tileType::MOUNTAIN): {
        return "Mountain";
    }case(tileType::MOUNTAIN_PEAK): {
        return "Mountain Peak";
    }case(tileType::WATER): {
        return "Water";
    }
    }
}



float waterLevel = 0.0f;
std::vector<Island> islands;

void createMapIslands() {

	int numIslands = calculateMapSize() / 4;

    for (int i = 0; i < numIslands; i++) {
        Island island;
        
        float tau = 6.28318f;
		float radius = calculateMapSize() * 0.9f;

        // Polar
		float degree = getRandomFloat(0, tau); // 0 to 2π
        float range = sqrt(getRandomFloat(0.0f, 1.0f)) * radius;

        // Cartesian conversion
		int x = cos(degree) * range;
		int y = sin(degree) * range;

		island.x = x;
		island.y = y;
        island.radius = getRandomInt(5000, 10000);
		islands.push_back(island);
    }
}

Island findClosestIsland(int x, int y) {
    Island closest;
    float closestDistance = std::numeric_limits<float>::infinity();
    for (auto& i : islands) {
        
        float xDist = i.x - x;
        float yDist = i.y - y;
        float dist = (xDist * xDist) + (yDist * yDist);

        if (dist < closestDistance) {
            closest = i;
            closestDistance = dist;
        }
    }
    return closest;
}

float NOISE_SCALE = 0.005f;
float NOISE_AMPLITUDE = 100.0f; // noise fluctuates between (-amplitude + waterleveladjust) to (amplitude + waterleveladjust), will be modified by falloff
float WATER_LEVEL_ADJUST = 100.0f; // higher starting point to offset water

float inverseCircleFalloff(float x, float y, int radius) {
    float dist = sqrt(x * x + y * y);
    float t = dist;
    return t;
}


int octaves = 4;
float persistence = 0.5f;
float lacunarity = 2.0f;

float calculateAltitude(int x, int y) {
	float size = NOISE_SCALE;
    float amplitude = NOISE_AMPLITUDE;
    float frequency = size;
    float baseAlt = 0.0f;
    for (int i = 0; i < octaves; i++) {
        baseAlt += perlin(x * frequency, y * frequency) * amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    baseAlt += WATER_LEVEL_ADJUST;
    auto closest = findClosestIsland(x, y);
    float dx = closest.x - x;
    float dy = closest.y - y;
	baseAlt -= inverseCircleFalloff(dx, dy, closest.radius);

	return baseAlt;

}

float getAltitude(int x, int y) {
    return calculateAltitude(x, y);
}


void setSeaLevel(float level) {
	waterLevel = level;
}

Tile assignTileTypes(int x, int y) {

    Tile tile;
    tile.altitude = calculateAltitude(x, y);


    if (tile.altitude > waterLevel + 135.0f) {
        tile.type = tileType::MOUNTAIN_PEAK;
        tile.blocked = true;
    }
    else if (tile.altitude > waterLevel + 105.0f) {
        tile.type = tileType::MOUNTAIN;
        tile.blocked = true;
    }
    else if (tile.altitude < waterLevel) {
        tile.type = tileType::WATER;
        tile.water = std::abs(waterLevel - tile.altitude);
        tile.anim.type = animType::WATER;
    }
    else if (tile.altitude < waterLevel + 5.0f) {
        tile.type = tileType::SAND;
    }
    else {
        tile.type = tileType::GRASS;
    }


    return tile;
}

void clearTile(int x, int y) {
    auto& registry = mainWorld.registry;

    auto entitiesOnTile = mainWorld.objectManager.getObjectsAt(x, y);

    for (entt::entity entity : entitiesOnTile) {
        if (registry.valid(entity)) {
            registry.destroy(entity);
        }
    }

    mainWorld.objectManager.clearTile(x, y);
}

void Tile::addObject_Clear(int x, int y, const std::string item) {
    clearTile(x, y);
    auto i = ObjectRegistry::getInstance().createInstance(item, mainWorld.registry);
    mainWorld.registry.emplace<Position>(i, x, y);
    mainWorld.objectManager.addObject(x, y, i);

    hasItems = true;
    //topItem = i;
}

void Tile::addObject(int x, int y, const std::string item, bool addToMove) {
    auto i = ObjectRegistry::getInstance().createInstance(item, mainWorld.registry);
    mainWorld.registry.emplace<Position>(i, x, y);
    mainWorld.objectManager.addObject(x, y, i);

    if (addToMove) {
		mainWorld.addItemToMove(i, x, y);
	}

    hasItems = true;
    //topItem = i;
}

void Tile::removeObject(int x, int y, entt::entity item) {
    mainWorld.objectManager.removeItem(x, y, item);
    mainWorld.registry.destroy(item);

    if (auto s = mainWorld.atStockpile(x, y)) {
        s->removeItem(x, y, item);
    }

    auto itemsOnTile = mainWorld.objectManager.getObjectsAt(x, y);
    if (itemsOnTile.empty()) {
        hasItems = false;
    }
}

void Tile::getTile(int x, int y) {
    //ObjectManager* manager = &mainWorld.objectManager;

    // Item adders

    // Probably a better way than a giant if-else chain
	float scale = 0.01f;

	float emeraldNoise = perlin(x * scale + 200.0f, y * scale + 200.0f);
    float goldNoise = perlin(x * scale + 400.0f, y * scale + 400.0f);
    float rubyNoise = perlin(x * scale + 600.0f, y * scale + 600.0f);
    float sapphireNoise = perlin(x * scale + 800.0f, y * scale + 800.0f);
    float rainbowNoise = perlin(x * scale + 1000.0f, y * scale + 1000.0f);

    float ironNoise = perlin(x * scale, y * scale);

	float oakNoise = perlin(x * 0.005f + 5000.0f, y * 0.005f + 5000.0f);
    float spruceNoise = perlin(x * 0.005f + 50000.0f, y * 0.005f + 50000.0f);

	float tallGrassNoise = perlin(x * 0.02f + 10000.0f, y * 0.02f + 10000.0f);

    if (type == tileType::GRASS) {

        float r = hashNoise(x, y, seed);

        if (tallGrassNoise > 0.3f) {
            if (r < 0.7f) {
                addObject_Clear(x, y, "Tall Grass");
            }
        }

        if (oakNoise > 0.3f || spruceNoise > 0.3f) {
            if (oakNoise > 0.3f /*&& altitude < waterLevel + 70.0f*/) {
                if (r < 0.1f) {
                    addObject_Clear(x, y, "Oak Tree");
                }
                else if (r < 0.101f) {
                    addObject_Clear(x, y, "Apple Tree");
                }
                else if (r < 0.13f) {
                    addObject_Clear(x, y, "Pebble");
                }
            }
            if (spruceNoise > 0.3f) {
                if (r < 0.1f) {
                    addObject_Clear(x, y, "Pine Tree");
                }
                else if (r < 0.11f) {
                    addObject_Clear(x, y, "Stick");
                }
                else if (r < 0.13f) {
                    addObject_Clear(x, y, "Pebble");
                }
            }
        }
        else {
            float oreThreshold = 0.5f;
            float oreSprinkler = hashNoise(x + 500.0f, y + 500.0f, seed);

            if (oreSprinkler < 0.6f) {
                if (ironNoise > oreThreshold - 0.1f) {
                    addObject_Clear(x, y, "Raw Iron");
                }
                if (rubyNoise > oreThreshold - 0.1f) {
                    addObject_Clear(x, y, "Copper");
                    anim.type = WHITE_BREATHE;
                }
                if (sapphireNoise > oreThreshold) {
                    addObject_Clear(x, y, "Coal");
                    anim.type = NONE;
                }
            }
        }

        float rock = hashNoise(x + 10000.0f, y + 10000.0f, seed);

        if (rock < 0.1f && altitude > waterLevel + 80.0f) {
            addObject_Clear(x, y, "Rock");
        }

        if (x == 1 && y == 0) {
			addObject_Clear(x, y, "Carpentry Bench");
        }

        if (x == 0 && y == 7) {
            addObject_Clear(x, y, "Apple Tree");
        }
        if (x == -7 && y == 7) {
            addObject_Clear(x, y, "Orange Tree");
        }
        if (x == 7 && y == 7) {
            addObject_Clear(x, y, "Lemon Tree");
        }
    }

    // Sets starting displays, subject to change
    auto display = getTileDisplay(type);

    character = display.character;
    color = display.color;

	walkable = getTileWalkable(type);
}

void Tile::changeTileType(tileType newType) {
    type = newType;
	character = getTileDisplay(type).character;
	color = getTileDisplay(type).color;
}

tileDisplay getTileDisplay(tileType type) {

    wchar_t c;
    switch (type) {
    case tileType::GRASS: {
        static const sf::String grass = L",.'~`";
        const std::vector<sf::Color> colors = {
            {95,195,20},
            {121,208,33},
            {161,223,80},
            {55, 174, 15}

        };
        int index = getRandomInt(0, grass.getSize() - 1);
        int shade = getRandomInt(100, 255);


		c = grass[index];

        sf::Color color = colors[getRandomInt(0, colors.size() - 1)];
        return { c, color };
    }
    case tileType::SOIL:
        c = L'=';
		return { c, sf::Color(84, 30, 0) };
    case tileType::WATER:
		return { L'≈', sf::Color(0, 0, 255) };
	case tileType::SAND:
		c = L':';
        return { c, sf::Color(255, 255, 0)};
    case tileType::MOUNTAIN:
        c = L'Δ';
        return { c, sf::Color(128,128,128)};
	case tileType::MOUNTAIN_PEAK:
		c = L'▲';
		return { c, sf::Color(200,200,200) };
    default:
        c = L'?';
        return { c, sf::Color::Red };
    }
}

bool getTileWalkable(tileType type) {

    switch (type) {
    case tileType::GRASS:
        return true;
	case tileType::SAND:
        return true;
    case tileType::SOIL:
        return true;
    default:
        return false;
    }
}

Tile& getTileRef(int x, int y) {

    int localX = (x % chunkDim + chunkDim) % chunkDim;
    int localY = (y % chunkDim + chunkDim) % chunkDim;

    int chunkX = static_cast<int>(std::floor((float)x / chunkDim));
    int chunkY = static_cast<int>(std::floor((float)y / chunkDim));

    Chunk& chunk = mainWorld.loadOrGenerateChunk(chunkX, chunkY);
    return chunk.tiles[localX][localY];
}

