#pragma once

#include "Villager.h"
#include "Dot.h"
#include "Item.h"
#include "mathUtils.h"


enum tileType {
    GRASS = 1,
    WATER = 2,
    SAND = 3,
    MOUNTAIN = 4,
	MOUNTAIN_PEAK = 5,
};

enum animType {
    NONE = 0,
	BREATHE = 1,
	RAINBOW = 2,
};

struct tileDisplay {
    std::vector<wchar_t> character;
    std::vector<sf::Color> color;
    tileDisplay(std::vector<wchar_t> c, std::vector<sf::Color> col) : character(c), color(col) {}
};

struct Island {
    int x, y;
    int dampen;
};

class Tile {
public:
    //int moistureLevel;
    wchar_t character;
    std::vector<wchar_t> charList;
    int charIndex;

	sf::Color origColorHolder;
    sf::Color color;

	animType animationType;
    std::vector<sf::Color> colorList;
    float altitude;
    float water = 0.0f;
	float flow = 0.0f;
    bool walkable;
    //item
    std::vector<std::unique_ptr<Item>> items;

    tileType type;

    int x, y;


    float animOffset = getRandomFloat(0.0f, 6.28318f);

	sf::String typeString;

    bool claimed = false;

    void update();

    void changeTileChar(sf::String string);
    void changeTileType(tileType type);


    void simulateWaterTile();

    void getTile(int x, int y);
	bool containsItem(const Item& item);
	void addItem(std::unique_ptr<Item> item);
	void removeItem(const Item& item);
    
};

tileDisplay getTileDisplay(tileType type);
bool getTileWalkable(Item* itemOnTile, tileType type);

void changeTileType(int x, int y, tileType type);

void changeTileChar(int x, int y, sf::String string);
void changeTileColor(int x, int y, sf::Color color);
void changeTileWalkable(int x, int y, bool walk);
void changeTileItem(int x, int y, std::unique_ptr<Item> item);

Tile assignTileTypes(int x, int y);

Tile& getTileRef(int x, int y);

void setSeaLevel(float level);
void createMapIslands();
void makeLake(int x, int y);
std::pair<int, int> makeRiver(int x, int y);

float findDistanceToDot(Dot dot, int x, int y);
Dot& findClosestDot(int x, int y);
float calculateAltitude(int x, int y);
float getAltitude(int x, int y);
void createVoronoiMap();

void addBasin(int x, int y);

extern std::vector<Island> islands;


std::vector<std::pair<int, int>> getWater();