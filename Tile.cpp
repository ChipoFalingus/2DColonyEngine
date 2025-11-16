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

std::vector<Dot> voronoiDots;
float waterLevel = -100.0f;



std::vector<Island> islands;



//this exists, ok sure
float hashNoise(int x, int y, int seed) {
    unsigned int h = x * 374761393u + y * 668265263u + seed * 374761393u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return (h & 0xFFFFFF) / float(0xFFFFFF); // 0.0f–1.0f
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
        //Makes several large islands
        Island island;
        island.x = getRandomInt(0, 0);
        island.y = getRandomInt(0, 0);
        island.dampen = getRandomInt(500, 500);
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

float calculateAltitude(Dot centerDot, int x, int y) {
    float size = 0.01f;
    //float baseAlt = perlin(x * size, y * size) * 100.0f;
    float baseAlt = 0.0f;
    auto closest = findClosestIsland(x, y);

    float dx = closest.x - x;
    float dy = closest.y - y;



    baseAlt -= inverseCircleFalloff(dx, dy, closest.dampen);
    return baseAlt + perlin(x * size, y * size) * 100.0f;
}

float getAltitude(int x, int y) {
    Dot closestDot = findClosestDot(x, y);
    return calculateAltitude(closestDot, x, y);
}


void setSeaLevel(float level) {
	waterLevel = level;
}

void makeLake(int x, int y) {
    float waterLevel = getTileRef(x, y).altitude;

    std::queue<std::pair<int, int>> toFill;
    std::unordered_set<long long> visited; // or hash of (x,y)
    auto hash = [](int x, int y) { return ((long long)x << 32) ^ (long long)y; };

    toFill.push({ x, y });
    visited.insert(hash(x, y));

    while (!toFill.empty()) {
        auto pair = toFill.front();
        toFill.pop();

        Tile& current = getTileRef(pair.first, pair.second);
        current.changeTileType(WATER);
        current.walkable = false;
        current.items.clear();

        for (auto& n : getNeighbors(pair.first, pair.second)) {
            int nx = n.first, ny = n.second;
            long long key = hash(nx, ny);
            if (visited.count(key)) continue;

            Tile& neighbor = getTileRef(nx, ny);
            // only fill neighbors that are <= the current water level
            if (neighbor.altitude <= waterLevel) {
                toFill.push({ nx, ny });
                visited.insert(key);
            }
        }
    }
}




//Inefficient rivers but they do the job
void makeRiver(int x, int y) {
    std::cout << "Make River" << std::endl;
    float currentAlt = getTileRef(x, y).altitude;

    while (currentAlt > -100.0f) {
        float tempAlt = currentAlt;

        std::vector<std::pair<int, int>> potentialNeighbors;

        for (auto& i : getNeighbors(x, y)) {
            Tile& neighbor = getTileRef(i.first, i.second);

            if (neighbor.altitude < currentAlt) {
                potentialNeighbors.push_back(i);
            }
        }

        if (potentialNeighbors.empty()) {
            makeLake(x, y);
            break;
        }

        auto chosenCoords = potentialNeighbors[getRandomInt(0, potentialNeighbors.size() - 1)];
        Tile& chosenNeighbor = getTileRef(chosenCoords.first, chosenCoords.second);

        currentAlt = chosenNeighbor.altitude;
        for (auto& i : getNeighbors(x, y)) {

            getTileRef(i.first, i.second).items.clear();

            getTileRef(i.first, i.second).walkable = false;
            getTileRef(i.first, i.second).changeTileType(WATER);
        }

		chosenNeighbor.items.clear();
        chosenNeighbor.walkable = false;

        chosenNeighbor.changeTileType(WATER);
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
    tile.altitude = calculateAltitude(findClosestDot(x, y), x, y);

    if (findClosestDot(x, y).biome == Biome::GRASS) {
        tile.type = GRASS;
        tile.typeString = "Grass";
    }
    else {
        tile.type = SAND;
        tile.typeString = "Sand";
    }


    if (tile.altitude < waterLevel) {
        tile.type = WATER;
        tile.typeString = "Water";
    }
    else if (tile.altitude < waterLevel + 5.0f) {
        tile.type = SAND;
        tile.typeString = "Sand";
    }
    else if (tile.altitude > waterLevel + 125.0f) {

        tile.type = MOUNTAIN;
        tile.typeString = "Mountain";
    }
    else {
        tile.type = GRASS;
        tile.typeString = "Grass";
    }

    return tile;
}


void Tile::getTile(int x, int y) {


    // Item adders
	float emeraldNoise = perlin(x * 0.05f + 200.0f, y * 0.05f + 200.0f);
    float goldNoise = perlin(x * 0.05f + 400.0f, y * 0.05f + 400.0f);
    float rubyNoise = perlin(x * 0.05f + 600.0f, y * 0.05f + 600.0f);
    float sapphireNoise = perlin(x * 0.05f + 800.0f, y * 0.05f + 800.0f);
    
    if (type == GRASS) {

        float r = hashNoise(x, y, seed);


        if (r < 0.04f) {
            if (r < 0.02f) {
                addItem(std::make_unique<Item>(tree));
                
            }
            else {
                addItem(std::make_unique<Item>(tree2));
            }
         }

        else if (r < 0.09f) {
            addItem(std::make_unique<Item>("Flower", L'*', sf::Color(getRandomInt(0, 255), getRandomInt(0, 255), getRandomInt(0, 255))));

        }
        else if (r < 0.14f) {
            addItem(std::make_unique<Item>(rock));
        }
    }

    /*items.clear();
    addItem(std::make_unique<Item>("DISPLAY", L'■', sf::Color(getRandomInt(0, 255), getRandomInt(0, 255), getRandomInt(0, 255))));
    animationType = BREATHE;*/

    if (x == 10 && y == 10) {
        items.clear();

        auto gun = std::make_unique<Item>(ASSAULT_RIFLE);
        Item* gunPtr = gun.get();        
        addItem(std::move(gun));      
        itemsToMove.push_back({ gunPtr, {x, y} });
        std::cout << gunPtr->name << std::endl;
    }



    // Sets starting displays, subject to change
    auto display = getTileDisplay(type);

    charList = display.character;
    charIndex = getRandomInt(0, charList.size() - 1);
    character = charList[charIndex];

    colorList = display.color;
    color = colorList[charIndex];
	origColorHolder = color;

    if (items.size() > 0) {
        walkable = getTileWalkable(items[0].get(), type);
    }
    else {
		walkable = getTileWalkable(nullptr, type);
    }
}


sf::Clock animClock;
sf::Clock colorClock;

void Tile::update() {


    // I dont know how it staggers the animations but Ill take it
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


    for (auto& i : items) {
        i->grow();
    }
}

void Tile::changeTileChar(sf::String string) {
}

void Tile::changeTileType(tileType type) {
    type = type;
	typeString = "Water";
    charList = getTileDisplay(type).character;
    colorList = getTileDisplay(type).color;
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


Tile& getTileRef(int x, int y) {
    int localX = (x % chunkDim + chunkDim) % chunkDim;
    int localY = (y % chunkDim + chunkDim) % chunkDim;

    int chunkX = static_cast<int>(std::floor((float)x / chunkDim));
    int chunkY = static_cast<int>(std::floor((float)y / chunkDim));

    Chunk& chunk = loadOrGenerateChunk(chunkX, chunkY);
    return chunk.tiles[localX][localY];
}

