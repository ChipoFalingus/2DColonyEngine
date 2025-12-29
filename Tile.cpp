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
#include <set>

// A lot of this stuff is world generation, so it should be moved to a separate file later
// A real big mess this all is :(

std::vector<Dot> voronoiDots;
float waterLevel = 0.0f;
std::vector<Island> islands;

std::vector<std::pair<int, int>> basins;
std::vector<std::pair<int, int>> newWater;

float hashNoise(int x, int y, int seed) {
    unsigned int h = x * 374761393u + y * 668265263u + seed * 374761393u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return (h & 0xFFFFFF) / float(0xFFFFFF);
    // I wish I could tell you how this works, the forums are a godsend
}



float findDistanceToDot(Dot dot, int x, int y) {
    float jitterAmount = 8.0f;

    float noiseX = perlin(x * 0.1f, y * 0.1f);
    float noiseY = perlin(x * 0.1f + 100.0f, y * 0.1f + 100.0f);

    float warpedX = x + noiseX * jitterAmount;
    float warpedY = y + noiseY * jitterAmount;

    float xDist = warpedX - dot.pos.x;
    float yDist = warpedY - dot.pos.y;

    return xDist * xDist + yDist * yDist;
}

//NEEDS OPTIMIZING
Dot& findClosestDot(int x, int y) {
    float shortestDist = INFINITY;
    Dot closestDot;
    for (int i = 0; i < voronoiDots.size(); i++) {

        float dist = findDistanceToDot(voronoiDots[i], x, y);
        if (shortestDist > dist) {
            shortestDist = dist;
            closestDot = voronoiDots[i];
        }
    }
    return closestDot;
}


void createVoronoiMap() {
    int numPoints = 15;
    voronoiDots.reserve(numPoints);


    float distPos = getRandomFloat(-100, 100);
    float distColor = getRandomFloat(0, 255);
    float distDirection = getRandomFloat(-1, 1);
    float distSpeed = getRandomFloat(0.1f, 1.0f);

    for (int i = 0; i < numPoints; i++) {
        Dot dot;
        dot.ID = getRandomInt(0, 21000000);
        dot.pos = { getRandomFloat(-100, 100), getRandomFloat(-100, 100) };
        dot.direction = { getRandomFloat(-1, 1), getRandomFloat(-1, 1) };
        dot.speed = { getRandomFloat(0.1f, 1.0f), getRandomFloat(0.1f, 1.0f) };
        dot.color = sf::Color(getRandomFloat(0.1f, 1.0f), getRandomFloat(0, 255), getRandomFloat(0, 255));
        if (getRandomInt(0, 5) == 6) {
            dot.biome = Biome::DESERT;
        }
        else {
            dot.biome = Biome::GRASS;
        }
        voronoiDots.push_back(dot);
    }
}

void createMapIslands() {
    for (int i = 0; i < 1; i++) {
        Island island;
        island.x = getRandomInt(0, 0);
        island.y = getRandomInt(0, 0);
        island.dampen = getRandomInt(500, 1000);
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

float inverseCircleFalloff(float x, float y, int radius) {
    return (x * x + y * y) / radius; // Larger radius = larger island
}

float calculateAltitude(int x, int y) {
    float size = 0.01f;
    //float baseAlt = perlin(x * size, y * size) * 100.0f;
    float baseAlt = 0.0f;
    auto closest = findClosestIsland(x, y);

    float dx = closest.x - x;
    float dy = closest.y - y;



    baseAlt -= inverseCircleFalloff(dx, dy, closest.dampen);
    //return baseAlt + 100.0f + perlin(x * size, y * size) * 100.0f;
	return baseAlt + 100.0f + perlin(x * size, y * size) * 100.0f;

}

float getAltitude(int x, int y) {
    Dot closestDot = findClosestDot(x, y);
    return calculateAltitude(x, y);
}


void setSeaLevel(float level) {
	waterLevel = level;
}

void makeLake(int x, int y) {
    std::cout << "Make Lake at " << x << ", " << y << std::endl;

    struct Node {
        int x, y;
        float altitude;
        bool operator>(const Node& other) const { return altitude > other.altitude; }
    };

    std::pair<int, int> rimLocation;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> toCheck;
    toCheck.push({ x, y, getTileRef(x, y).altitude });

    //float rimHeight = getTileRef(startX, startY).altitude;
    float rimHeight = INFINITY;
    std::set<std::pair<int, int>> finalLake;

    int i = 0;
    //Explore tiles until it finds a tile lower 
    while (!toCheck.empty()) {
        Node current = toCheck.top();
        toCheck.pop();


        if (finalLake.count({ current.x, current.y })) continue;
        finalLake.insert({ current.x, current.y });

        for (auto& n : getNeighbors(current.x, current.y)) {
            int nx = n.first;
            int ny = n.second;

            if (finalLake.count({ nx, ny })) {
                continue;
            }

            Tile& neighbor = getTileRef(nx, ny);

            // Always add neighbor to queue for further exploration


            // Only update rim if neighbor is higher than current
            if (neighbor.altitude > current.altitude && neighbor.altitude < rimHeight) {
                toCheck.push({ nx, ny, neighbor.altitude });

            }
            else {
                rimLocation = { nx, ny };
                rimHeight = std::min(rimHeight, neighbor.altitude);
                //std::cout << "New Rim Height: " << rimHeight << " at (" << nx << ", " << ny << ")" << std::endl;

            }
        }

        i++;
        //std::cout << "Checked Tiles: " << i << std::endl;
    }



    for (auto& i : finalLake) {
        Tile& tile = getTileRef(i.first, i.second);
        tile.items.clear();
        tile.walkable = false;
        tile.changeTileType(WATER);
        //tile.addItem(std::make_unique<Item>("Lake", L'$', sf::Color::Red));
    }

    float lowAdj = INFINITY;
    std::pair<int, int> lowAdjLocation;
    for (auto& i : getNeighbors(rimLocation.first, rimLocation.second)) {
        if (getTileRef(i.first, i.second).altitude < lowAdj && getTileRef(i.first, i.second).type == GRASS) {
            lowAdj = getTileRef(i.first, i.second).altitude;
            lowAdjLocation = { i.first, i.second };
        }
    }

    //makeRiver(rim.first, rim.second);

}

void addBasin(int x, int y) {
    auto target = std::make_pair(x, y);
    auto it = std::find(basins.begin(), basins.end(), target);

    if (it == basins.end()) {
        std::cout << "Basin low at " << x << ", " << y << std::endl;
        basins.push_back(target);
        //waterTiles.push_back(target);
    }
}



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

    if (findClosestDot(x, y).biome == Biome::GRASS) {
        tile.type = GRASS;
        tile.typeString = "Grass";
    }
    else {
        tile.type = SAND;
        tile.typeString = "Sand";
    }


    if (tile.altitude > waterLevel + 135.0f) {
        tile.type = MOUNTAIN_PEAK;
        tile.typeString = "Mountain Peak";
    }
    else if (tile.altitude > waterLevel + 105.0f) {
        tile.type = MOUNTAIN;
        tile.typeString = "Mountain";

		

    }
    else if (tile.altitude < waterLevel) {
        tile.type = WATER;
        tile.typeString = "Water";
        tile.water = std::abs(waterLevel - tile.altitude);
    }
    else if (tile.altitude < waterLevel + 5.0f) {
        tile.type = SAND;
        tile.typeString = "Sand";
    }
    else {
        tile.type = GRASS;
        tile.typeString = "Grass";
    }

    return tile;
}


void Tile::getTile(int x, int y) {


    // Item adders

    // Probably a better way than a giant if-else chain

	float emeraldNoise = perlin(x * 0.05f + 200.0f, y * 0.05f + 200.0f);
    float goldNoise = perlin(x * 0.05f + 400.0f, y * 0.05f + 400.0f);
    float rubyNoise = perlin(x * 0.05f + 600.0f, y * 0.05f + 600.0f);
    float sapphireNoise = perlin(x * 0.05f + 800.0f, y * 0.05f + 800.0f);

    Item tree = *ItemRegistry::getInstance().get("Oak Tree");
    Item tree2 = *ItemRegistry::getInstance().get("Pine Tree");
	Item tree3 = *ItemRegistry::getInstance().get("Birch Tree");
    Item rock = *ItemRegistry::getInstance().get("Rock");
    Item wood = *ItemRegistry::getInstance().get("Wood");
    
	Item carp = *ItemRegistry::getInstance().get("Carpentry Bench");

    if (type == GRASS) {

        float r = hashNoise(x, y, seed);


        if (r < 0.04f && altitude < waterLevel + 70.0f) {
            if (r < 0.013f) {
                addItem(std::make_unique<Item>(tree));
            }
            else if (r < 0.026f) {
                addItem(std::make_unique<Item>(tree2));
            }
            else {
                addItem(std::make_unique<Item>(tree3));
                
            }
         }

        else if (r < 0.09f && altitude < waterLevel + 70.0f) {
            addItem(std::make_unique<Item>("Flower", L'*', sf::Color(getRandomInt(0, 255), getRandomInt(0, 255), getRandomInt(0, 255))));

        }
        else if (r < 0.24f && altitude > waterLevel + 80.0f) {
            addItem(std::make_unique<Item>(rock));
        }
    }



	float noise = hashNoise(x + 500, y + 500, seed);

    if (noise < 0.3f) {
		//water += 1.0f;
		//newWater.push_back({ x, y });
    }

    if (x == 2 && y == 2) {
        items.clear();
        addItem(std::make_unique<Item>(carp));
    }

    /*items.clear();
    addItem(std::make_unique<Item>("DISPLAY", L'■', sf::Color(getRandomInt(0, 255), getRandomInt(0, 255), getRandomInt(0, 255))));
    animationType = BREATHE;*/

    //std::vector<Item> itempool = {wood, tree, tree2, rock, iron, emerald, gold, ruby, sapphire, topaz, diamond, flower, wheatSeed, chest, wall, stonePath, woodenFence};
    /*std::vector<Item> itempool = { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, xx, yy, z };


    if (x == 100 && y == 100) {
        items.clear();


        for (int i = 0; i < 4800; i++) {
			Item chosenitem = itempool[getRandomInt(0, itempool.size() - 1)];
            auto item = std::make_unique<Item>(chosenitem);
            Item* ptr = item.get();
            addItem(std::move(item));
            itemsToMove.push_back({ ptr, {x, y} });
        }
       

    }*/



    // Sets starting displays, subject to change
    auto display = getTileDisplay(type);

    charList = display.character;
    charIndex = getRandomInt(0, charList.size() - 1);
    character = charList[charIndex];

    colorList = display.color;
    color = colorList[charIndex];
	origColorHolder = color;

	this->x = x;
	this->y = y;

    if (items.size() > 0) {
        walkable = getTileWalkable(items[0].get(), type);
    }
    else {
		walkable = getTileWalkable(nullptr, type);
    }
}


sf::Clock animClock;
sf::Clock colorClock;


enum SedimentState {
    DEPOSIT,
    TRANSPORT,
    ERODE
};

SedimentState hjulstromDiagram(float flowVelocity, float grainSize) {
    if (flowVelocity < 0.1f) {
        return DEPOSIT;
		std::cout << "Deposit" << std::endl;
    }
    else if (flowVelocity < 1.0f) {
        if (grainSize < 0.05f) {
            return TRANSPORT;
            std::cout << "Transport" << std::endl;
        }
        else {
            return DEPOSIT;
            std::cout << "Deposit" << std::endl;
        }
    }
    else {
        if (grainSize < 0.1f) {
            return ERODE;
            std::cout << "Erode" << std::endl;
        }
        else {
            return TRANSPORT;
            std::cout << "Transport" << std::endl;
        }
    }

}


/*

UPDATING THE EROSION SIMULATION:

Figure out where the water will flow
Update positions of water
Calculate deltaHeight
Calculate sediment capacity (higher when water is moving faster and has more volume)
If capacity > current sediment, erode
If water slows, deposit


*/

void Tile::simulateWaterTile() {
       
    for (auto& i : basins) {
        if (i.first == x && i.second == y) {
            if (water < 500.0f) {
                water += 200.5f;
            }
        }
    }

	//water += 1.0f;

    /*if (altitude < waterLevel) {
        water = 0.0f;
        return;
    }*/


    float grainSize = 0.002f; // In meters

    float surface = altitude + water;

    for (auto& n : getNeighbors(x, y)) {
        Tile& other = getTileRef(n.first, n.second);

        float otherSurface = other.altitude + other.water;

        if (surface > otherSurface) {

            float flow = (surface - otherSurface) * 0.5f;

            flow = std::min(flow, water);
			this->flow = flow;

            water -= flow;
            other.water += flow;

            surface = altitude + water;

            // Hjulstrom's Graph implementation

			float flowVelocity = flow * 10.0f;

            float erosionStrength = 0.0005f;
			float depositionStrength = 0.0003f;

			SedimentState state = hjulstromDiagram(flowVelocity, grainSize);

            if (state == ERODE) {
                altitude -= flow * erosionStrength;
            }
            else if (state == DEPOSIT) {
                other.altitude += flow * depositionStrength;
			}

            if (flow >= 1.0f) {
                other.items.clear();
            }
			
            //if (water < 0.001f) {
            //    // Tiny water, we'll just pretend the soil soaked it up
            //    water = 0.0f;
            //}
            
        }
    }
}

std::vector<std::pair<int, int>> getWater() {
	return newWater;
}

struct RGB {
    float r, g, b;
};

RGB HSVtoRGB(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float r, g, b;

    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return { r + m, g + m, b + m };
}

void Tile::update() {

    // This method doesn't work if the tiles aren't rendered already, a real pain

    // I dont know how it staggers the animations but Ill take it
    // ^ Future me figured it out and its atrocious to think about
    
    // Needs some work, probably should be independent from item animations


    if (animClock.getElapsedTime().asSeconds() > 0.001f) {
        if (charIndex >= charList.size() - 1) {
            charIndex = 0;
        }
        else {
            charIndex++;
        }
        animClock.restart();
        character = charList[charIndex];
        color = colorList[charIndex];
    }

    if (enableWater) {
        simulateWaterTile();
    }
    


    // Early exit, all other anims are based on items
    if (items.size() == 0) {
        return;
    }

    // Animated Colors / Animated Displays (will be added later)
    if (animationType == BREATHE) {
        float time = colorClock.getElapsedTime().asSeconds();

		sf::Color itemColor = items[0]->baseColor;

        float min = 0.2f;
        float max = 1.0f;

        float intensity = min + (max - min) * ((sin(time + animOffset) + 1.0f) / 2.0f);

        color.r = static_cast<sf::Uint8>(itemColor.r * intensity);
        color.g = static_cast<sf::Uint8>(itemColor.g * intensity);
        color.b = static_cast<sf::Uint8>(itemColor.b * intensity);

		items[0]->displayColor = color;
    }

    if (animationType == RAINBOW) {
        float time = colorClock.getElapsedTime().asSeconds();
        float hue = fmod((time * 60.0f) + animOffset * 60.0f, 360.0f); // Convert offset to degrees
        RGB rgb = HSVtoRGB(hue, 1.0f, 1.0f);
        color.r = static_cast<sf::Uint8>(rgb.r * 255);
        color.g = static_cast<sf::Uint8>(rgb.g * 255);
		color.b = static_cast<sf::Uint8>(rgb.b * 255);

        //animOffset = 0.0f;

		items[0]->displayColor = color;
    }

    
    


    for (auto& i : items) {
        i->grow();
    }
}

void Tile::changeTileChar(sf::String string) {
}

void Tile::changeTileType(tileType newType) {
    type = newType;
	typeString = "Water";
    charList = getTileDisplay(newType).character;
    colorList = getTileDisplay(newType).color;
    character = charList[0];
    color = colorList[0];
}

bool Tile::containsItem(const Item& item) {
    for (auto& i : items) {
        if (i->name == item.name) {
			return true;
        }
    }
    return false;
}

void Tile::addItem(std::unique_ptr<Item> item) {
    items.insert(items.begin(), std::move(item));
}

void Tile::removeItem(const Item& item) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if ((*it)->name == item.name) {
            items.erase(it);
            return;
        }
    }
}

tileDisplay getTileDisplay(tileType type) {

    // Bad approach, should have animation timings stored somewhere
    wchar_t c;
    switch (type) {
    case GRASS: {
        static const sf::String grass = L",.'~`";
        int index = getRandomInt(0, grass.getSize() - 1);
        int shade = getRandomInt(100, 255);

		c = grass[index];

        return { {c}, {sf::Color(1, shade, 1)}};
    }
    case WATER:
        return { {L'≈', L'≈', L'≈', L'≈',
            L'≈', L'≈', L'≈', L'≈',
            L'≈', L'≈', L'≈', L'≈',
            L'≈', L'≈', L'≈', L'≈',
            L'≈', L'≈', L'~'},
            {sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, 
            sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, 
            sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, 
            sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, sf::Color::Blue, 
            sf::Color::Blue, sf::Color::Blue, sf::Color(255, 255, 255)}
        };
	case SAND:
		c = L':';
        return { {c}, {sf::Color(255, 255, 0)}};
    case MOUNTAIN:
        c = L'Δ';
        return { {c}, {sf::Color(128,128,128)}};
	case MOUNTAIN_PEAK:
		c = L'▲';
		return { {c}, {sf::Color(200,200,200)} };
    default:
        c = L'?';
        return { {c}, {sf::Color::Red}};
    }
}

bool getTileWalkable(Item* itemOnTile, tileType type) {

	// Will probably put this in item class later

    if (itemOnTile) {
        if (itemOnTile->name == "Tr") {
            return false;
        }
        else if (itemOnTile->name == "a") {
            return false;
        }
        else {
            return true;
        }
    }


    switch (type) {
    case GRASS:
        return true;
    case WATER:
        return false;
	case SAND:
        return true;
    default:
        return false;
    }
}


// The following 5 methods need to be gone
void changeTileType(int x, int y, tileType type) {
    Tile& tile = getTileRef(x, y);
    tile.type = type;
}

void changeTileChar(int x, int y, wchar_t string) {
    Tile& tile = getTileRef(x, y);
    tile.character = string;
}

void changeTileColor(int x, int y, sf::Color color) {
    Tile& tile = getTileRef(x, y);
    tile.color = color;
}

void changeTileWalkable(int x, int y, bool walk) {
    Tile& tile = getTileRef(x, y);
    tile.walkable = walk;
}

//void changeTileItem(int x, int y, std::unique_ptr<Item> item) {
//    Tile& tile = getTileRef(x, y);
//    tile.itemOnTile = std::move(item);
//}

// The backbone of everything ever
Tile& getTileRef(int x, int y) {
    int localX = (x % chunkDim + chunkDim) % chunkDim;
    int localY = (y % chunkDim + chunkDim) % chunkDim;

    int chunkX = static_cast<int>(std::floor((float)x / chunkDim));
    int chunkY = static_cast<int>(std::floor((float)y / chunkDim));

    Chunk& chunk = loadOrGenerateChunk(chunkX, chunkY);
    return chunk.tiles[localX][localY];
}

