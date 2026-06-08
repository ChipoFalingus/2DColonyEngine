#pragma once

#include "TileDisplay.h"
#include "Object.h"
#include "mathUtils.h"


enum class tileType : uint8_t {
    GRASS,
    SOIL,
    WATER,
    SAND,
    MOUNTAIN,
	MOUNTAIN_PEAK,
};

enum animType {
    NONE = 0,
    STATIC = 1,
    BREATHE = 2,
    RAINBOW = 3,
    RED_X = 4,
    WHITE_BREATHE = 5,
    WATER = 6,
};

struct Animation {
    animType type;
    bool isOtherChar = false;
};

enum Biome {
    DESERT,
    DESERT_SCRUB,
    THORN_WOODLAND,
    STEPPE,
    DRY_FOREST,
    MOIST_FOREST,
    WET_FOREST,
    RAIN_FOREST,
    DRY_TUNDRA,
    MOIST_TUNDRA,
    WET_TUNDRA,
    RAIN_TUNDRA,
    SNOW
};

struct Island {
    int x, y;
    int radius;
};

class Tile {
private:
public:
    wchar_t character;
    sf::Color color;

    uint8_t water;
    int16_t altitude;
    bool walkable;

    bool blocked = false;

	bool markedForHarvest = false;

    //item
    std::vector<std::shared_ptr<Object>> items;

    tileType type;

    Animation anim;

    float animOffset = getRandomFloat(0, 2 * 3.1415926);
    

    int region = -1;

    void update();

    void changeTileChar(sf::String string);
    void changeTileType(tileType type);

    void getTile(int x, int y);
    bool containsItem(const std::string& item);
    void addObject(std::string itemName);
	void addObject(std::shared_ptr<Object> item);
    void removeItem(std::shared_ptr<Object> item, int x, int y);
    void removeItem(std::string item);
};

std::string typeToString(tileType type);

tileDisplay getTileDisplay(tileType type);
bool getTileWalkable(Object* itemOnTile, tileType type);

Tile assignTileTypes(int x, int y);

Tile& getTileRef(int x, int y);

void setSeaLevel(float level);
void createMapIslands();
void makeLake(int x, int y);
std::pair<int, int> makeRiver(int x, int y);

float calculateAltitude(int x, int y);
float getAltitude(int x, int y);

inline std::vector<std::pair<int, int>> getNeighbors(int x, int y) {
    return { {x - 1, y}, {x, y - 1}, {x + 1, y}, {x, y + 1} };
}

extern std::vector<Island> islands;