#include <iostream>
#include <SFML/Graphics.hpp>
#include <queue>
#include <unordered_set>

#include "CreatureUtils.h"
#include "mathUtils.h"
#include "Vec2.h"
#include "Tile.h"
#include "Chunk.h"
#include "Crop.h"
#include "Gun.h"
#include "Tool.h"
#include "Light.h"
#include "Globals.h"
#include "Game.h"
#include "FoliageCrop.h"
#include "Furnace.h"
#include "Spawner.h"
#include "Structure.h"
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

	int numIslands = 300;

    for (int i = 0; i < numIslands; i++) {
        Island island;
        
        float tau = 6.28318f;
		float radius = 1000.0f;

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

//void makeLake(int x, int y) {
//    std::cout << "Make Lake at " << x << ", " << y << std::endl;
//
//    struct Node {
//        int x, y;
//        float altitude;
//        bool operator>(const Node& other) const { return altitude > other.altitude; }
//    };
//
//    std::pair<int, int> rimLocation;
//    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> toCheck;
//    toCheck.push({ x, y, getTileRef(x, y).altitude });
//
//    //float rimHeight = getTileRef(startX, startY).altitude;
//    float rimHeight = INFINITY;
//    std::set<std::pair<int, int>> finalLake;
//
//    int i = 0;
//    //Explore tiles until it finds a tile lower 
//    while (!toCheck.empty()) {
//        Node current = toCheck.top();
//        toCheck.pop();
//
//
//        if (finalLake.count({ current.x, current.y })) continue;
//        finalLake.insert({ current.x, current.y });
//
//        for (auto& n : getNeighbors(current.x, current.y)) {
//            int nx = n.first;
//            int ny = n.second;
//
//            if (finalLake.count({ nx, ny })) {
//                continue;
//            }
//
//            Tile& neighbor = getTileRef(nx, ny);
//
//            // Only update rim if neighbor is higher than current
//            if (neighbor.altitude > current.altitude && neighbor.altitude < rimHeight) {
//                toCheck.push({ nx, ny, neighbor.altitude });
//
//            }
//            else {
//                rimLocation = { nx, ny };
//                rimHeight = std::min(rimHeight, neighbor.altitude);
//
//            }
//        }
//
//        i++;
//    }
//
//
//
//    for (auto& i : finalLake) {
//        Tile& tile = getTileRef(i.first, i.second);
//        tile.items.clear();
//        tile.walkable = false;
//        tile.changeTileType(WATER);
//        //tile.addItem(std::make_unique<Item>("Lake", L'$', sf::Color::Red));
//    }
//
//    float lowAdj = INFINITY;
//    std::pair<int, int> lowAdjLocation;
//    for (auto& i : getNeighbors(rimLocation.first, rimLocation.second)) {
//        if (getTileRef(i.first, i.second).altitude < lowAdj && getTileRef(i.first, i.second).type == GRASS) {
//            lowAdj = getTileRef(i.first, i.second).altitude;
//            lowAdjLocation = { i.first, i.second };
//        }
//    }
//
//    //makeRiver(rim.first, rim.second);
//
//}



// Inefficient rivers but they do the job
std::pair<int, int> makeRiver(int x, int y) {
    //std::cout << "Make River at " << x << ", " << y << std::endl;
    float currentAlt = getTileRef(x, y).altitude;

    while (currentAlt > -100.0f) {
        float tempAlt = currentAlt;

        std::vector<std::pair<int, int>> potentialNeighbors;

		float minAlt = currentAlt;
        for (auto& i : getNeighbors(x, y)) {
            Tile& neighbor = getTileRef(i.first, i.second);

            if (neighbor.altitude < minAlt) {
                potentialNeighbors.push_back(i);
				minAlt = neighbor.altitude;
            }
        }

        if (potentialNeighbors.empty()) {
            //makeLake(x, y);
			return { x, y };
            break;
        }

        auto chosenCoords = potentialNeighbors[getRandomInt(0, potentialNeighbors.size() - 1)];
        Tile& chosenNeighbor = getTileRef(chosenCoords.first, chosenCoords.second);

        currentAlt = chosenNeighbor.altitude;
        /*for (auto& i : getNeighbors(x, y)) {

            getTileRef(i.first, i.second).items.clear();

            getTileRef(i.first, i.second).walkable = false;
            getTileRef(i.first, i.second).changeTileType(WATER);
        }

		chosenNeighbor.items.clear();
        chosenNeighbor.walkable = false;

        chosenNeighbor.changeTileType(WATER);*/
        x = chosenCoords.first;
        y = chosenCoords.second;

        if (tempAlt == currentAlt) {
            break;
        }
    }
}

Tile assignTileTypes(int x, int y) {
    // Base tile types, theres no way to change them yet

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


void Tile::getTile(int x, int y) {
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
                items.clear();
                addObject("Tall Grass");
            }
		}

        if (oakNoise > 0.3f || spruceNoise > 0.3f) {
            if (oakNoise > 0.3f /*&& altitude < waterLevel + 70.0f*/) {
                if (r < 0.1f) {
                    items.clear();
                    addObject("Oak Tree");
                }
                else if (r < 0.101f) {
                    items.clear();
                    auto item = ObjectRegistry::getInstance().get("Apple Tree");
                    auto spawner = static_cast<FoliageCrop*>(item.get());
                    spawner->x = x;
                    spawner->y = y;
                    addObject(item);
                    tiles.push_back({ x, y });
                   // addObject("Stick");
                }
                else if (r < 0.13f) {
                    items.clear();
                   // addObject("Pebble");
                }
            }
            if (spruceNoise > 0.3f) {
                if (r < 0.1f) {
                    items.clear();
                    addObject("Pine Tree");
                }
                else if (r < 0.11f) {
                    items.clear();
                   //addObject("Stick");
                }
                else if (r < 0.13f) {
                    items.clear();
                    //addObject("Pebble");
                }
            }
        }
        else {
            float oreThreshold = 0.5f;
            float oreSprinkler = hashNoise(x + 500.0f, y + 500.0f, seed);

            if (oreSprinkler < 0.6f) {
                if (ironNoise > oreThreshold - 0.1f) {
                    items.clear();
                    addObject("Raw Iron");
                }
                if (rubyNoise > oreThreshold - 0.1f) {
                    items.clear();
                    addObject("Copper");
                    anim.type = WHITE_BREATHE;
                }
                if (sapphireNoise > oreThreshold) {
                    items.clear();
                    addObject("Coal");
                    anim.type = NONE;
                }
            }
        }
        
		float rock = hashNoise(x + 10000.0f, y + 10000.0f, seed);
        
        if (rock < 0.1f && altitude > waterLevel + 80.0f) {
            items.clear();
            addObject("Rock");
        }

        float outposts = hashNoise(x + 10000.0f, y + 10000.0f, seed);

        /*if (outposts < 0.000005f) {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Outpost");
            auto spawner = static_cast<Spawner*>(item.get());
            spawner->x = x;
			spawner->y = y;
            addObject(item);

			tiles.push_back({ x, y });
        }*/

        /*if (x == -20 && y == 20) {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Outpost");
            auto spawner = static_cast<Spawner*>(item.get());
            spawner->x = x;
            spawner->y = y;
            addObject(item);

            tiles.push_back({ x, y });
        }*/

        /*if (x == 10 && y == 10) {
           items.clear();
           auto item = ObjectRegistry::getInstance().get("Apple Tree");
           auto spawner = static_cast<FoliageCrop*>(item.get());
           spawner->x = x;
           spawner->y = y;
           addObject(item);
        }
        if (x == 0 && y == 10) {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Orange Tree");
            auto spawner = static_cast<FoliageCrop*>(item.get());
            spawner->x = x;
            spawner->y = y;
            addObject(item);
        }
        if (x == -10 && y == 10) {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Lemon Tree");
            auto spawner = static_cast<FoliageCrop*>(item.get());
            spawner->x = x;
            spawner->y = y;
            addObject(item);
        }*/

        float scrapNoise = hashNoise(x + 20000.0f, y + 20000.0f, seed);

        /*if (scrapNoise < 0.005f) {
            items.clear();
            addObject("Scrap Metal");
        }*/

        int range = 10;

        //if (
        //    (x == 5 || x == -5 || y == 5 || y == -5) &&
        //    (x >= -5 && x <= 5 && y >= -5 && y <= 5) && x != 0
        //    ) {
        //    items.clear();
        //    auto item = ObjectRegistry::getInstance().get("Wooden Wall");
        //    auto i = static_cast<Structure*>(item.get());
        //    i->x = x;
        //    i->y = y;
        //    addObject(item);
        //    blocked = true;
        //}
        ///*else if (
        //    (x == 5 || x == -5 || y == 5 || y == -5) &&
        //    (x >= -5 && x <= 5 && y >= -5 && y <= 5)
        //    ) {
        //    items.clear();
        //    auto item = ObjectRegistry::getInstance().get("Wooden Gate");
        //    auto i = static_cast<Gate*>(item.get());
        //    i->x = x;
        //    i->y = y;
        //    addObject(item);
        //    blocked = true;
        //}*/

        //if (
        //    (x >= -4 && x <= 4 && y >= -4 && y <= 4)
        //    ) {
        //    items.clear();
        //    addObject("Stone Floor");
        //}

        //if ((x == 4 && y == 4) ||
        //    (x == -4 && y == 4) ||
        //    (x == 4 && y == -4) ||
        //    (x == -4 && y == -4)) {
        //    items.clear();
        //    addObject("Bed");
        //}
        //if ((x == 3 && y == 4) ||
        //    (x == -3 && y == 4) ||
        //    (x == 3 && y == -4) ||
        //    (x == -3 && y == -4)) {
        //    items.clear();
        //    addObject("Wooden Chair");
        //}
        //if ((x == 4 && y == 3) ||
        //    (x == -4 && y == 3) ||
        //    (x == 4 && y == -3) ||
        //    (x == -4 && y == -3)) {
        //    items.clear();
        //    addObject("Wooden Chair");
        //}

        //if (x == 10 && y >= 0 && y <= 5) {
        //    items.clear();
        //    addObject("Wooden Chair");
        //}
        //if (x == 9 && y >= 0 && y <= 5) {
        //    items.clear();
        //    addObject("Wooden Table");
        //}

        //if (x == -0 && y == 0) {
        //    items.clear();
        //    addObject("Furnace");
        //}
        //if (x == -1 && y == 0) {
        //    items.clear();
        //    addObject("Carpentry Bench");
        //}
        //if (x == 1 && y == 0) {
        //    items.clear();
        //    addObject("Anvil");
        //}

    }

    /*if (x > 3 && x < 5 && y > 5 && y < 9) {
        items.clear();
        auto item = ObjectRegistry::getInstance().get("Wooden Chair");
        addObject(item);
	}
    if (x > 2 && x < 4 && y > 5 && y < 9) {
        items.clear();
        auto item = ObjectRegistry::getInstance().get("Wooden Table");
        addObject(item);
    }*/

    /*if (x > -3 && x < 3 && y > -3 + 10 && y < 3 + 10) {
		int num = getRandomInt(0, 0);
        if (num == 0) {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Wheat Seeds");
            addObject(item);
            mainWorld.addItemToMove(item, x, y);
        }
        else if (num == 1) {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Carrot Seeds");
            addObject(item);
            mainWorld.addItemToMove(item, x, y);
        }
        else {
            items.clear();
            auto item = ObjectRegistry::getInstance().get("Potato Seeds");
            addObject(item);
            mainWorld.addItemToMove(item, x, y);
        }
        
    }*/
   /* if (x == 0 && y == 0) {
		addObject("Carpentry Bench");
    }*/

    if (x > -10 && x < 10 && y > -10 && y < 10) {
        auto item = ObjectRegistry::getInstance().get("Wheat");
		//auto c = static_cast<Crop*>(item.get());
        items.clear();
		addObject(item);
		tiles.push_back({ x, y });
    }

    // Sets starting displays, subject to change
    auto display = getTileDisplay(type);

    character = display.chars[0];

    color = display.colors[0];

    if (items.size() > 0) {
        walkable = getTileWalkable(items[0].get(), type);
    }
    else {
		walkable = getTileWalkable(nullptr, type);
    }
}

void Tile::update() {
    if (items.size() == 0) {
        return;
    }

    for (auto& i : items) {
        if (i->type == Type::Foliage_Crop) {
			FoliageCrop* crop = static_cast<FoliageCrop*>(i.get());
            crop->spawnProduce();
        }

        if (i->type == Type::Crop) {
            Crop* cropPtr = static_cast<Crop*>(i.get());
            cropPtr->grow();
        }

        if (i->type == Type::Spawner) {
            Spawner* spawnerPtr = static_cast<Spawner*>(i.get());
            spawnerPtr->update();
        }
		
        if (i->type == Type::Furnace) {
            Furnace* furnacePtr = static_cast<Furnace*>(i.get());
			furnacePtr->cook();
        }
    }
}

void Tile::changeTileChar(sf::String string) {
}

void Tile::changeTileType(tileType newType) {
    type = newType;
	character = getTileDisplay(type).chars[0];
	color = getTileDisplay(type).colors[0];
}

bool Tile::containsItem(const std::string& item) {
    for (auto& i : items) {
        if (i->name == item) {
			return true;
        }
    }
    return false;
}


void Tile::addObject(std::string itemName) {
	auto item = ObjectRegistry::getInstance().get(itemName);
    if (item) {
		addObject(item);
	}
}

void Tile::addObject(std::shared_ptr<Object> item) {
    items.insert(items.begin(), item);
}

void Tile::removeItem(std::shared_ptr<Object> item, int x, int y) {
    anim.type = animType::NONE;
    auto stockpile = mainWorld.atStockpile(x, y);
    if (stockpile) {
        stockpile->removeItem(x, y, item);
    }
    for (auto it = items.begin(); it != items.end(); ++it) {
        if ((*it)->name == item->name) {
            items.erase(it);
            return;
        }
    }
}

void Tile::removeItem(std::string item) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if ((*it)->name == item) {
            items.erase(it);
            return;
        }
    }
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
        return { {c}, {color}};
    }
    case tileType::SOIL:
        c = L'=';
		return { {c}, {sf::Color(84, 30, 0)} };
    case tileType::WATER:
		return { {L'≈'}, {sf::Color(0, 0, 255)} };
	case tileType::SAND:
		c = L':';
        return { {c}, {sf::Color(255, 255, 0)}};
    case tileType::MOUNTAIN:
        c = L'Δ';
        return { {c}, {sf::Color(128,128,128)}};
	case tileType::MOUNTAIN_PEAK:
		c = L'▲';
		return { {c}, {sf::Color(200,200,200)} };
    default:
        c = L'?';
        return { {c}, {sf::Color::Red}};
    }
}

bool getTileWalkable(Object* itemOnTile, tileType type) {

	// Will probably put this in item class later

    if (itemOnTile) {
        if (itemOnTile->name == "Wooden Wall") {
            return false;
        }
        else if (itemOnTile->name == "Rck") {
            return false;
        }
        else {
            return true;
        }
    }


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

// The backbone of everything ever
Tile& getTileRef(int x, int y) {

    int localX = (x % chunkDim + chunkDim) % chunkDim;
    int localY = (y % chunkDim + chunkDim) % chunkDim;

    int chunkX = static_cast<int>(std::floor((float)x / chunkDim));
    int chunkY = static_cast<int>(std::floor((float)y / chunkDim));

    Chunk& chunk = mainWorld.loadOrGenerateChunk(chunkX, chunkY);
    return chunk.tiles[localX][localY];
}

