#pragma once

#include "World/TileDisplay.h"
#include "Entities/Object.h"
#include "Utility/mathUtils.h"


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
    FIRE = 7,
    SPEECH_BUBBLE = 8,
    Z = 9,
    GUN_SHOT = 10,
};

struct Animation {
    animType type = animType::NONE;
    bool isOtherChar = false;

    float timer = 0.0f;
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
    glm::vec3 color;

    uint8_t water;

    int16_t altitude;
    int16_t region = -1;

    bool walkable = false;
    bool blocked = false;
	bool markedForHarvest = false;

    tileType type;

    Animation anim;

    float animOffset = getRandomFloat(0, 2 * 3.1415926);
    
    void changeTileType(tileType type);
    void getTile(int x, int y);

    void addObject_Clear(int x, int y, const std::string item);
	void addObject(int x, int y, const std::string item, bool addToMove = false);

	void removeObject(int x, int y, entt::entity item);

    void setAnimType(animType type) {
        anim.type = type;
    }
};

std::string typeToString(tileType type);

tileDisplay getTileDisplay(tileType type);
bool getTileWalkable(tileType type);

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