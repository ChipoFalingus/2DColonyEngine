#pragma once
#include <vector>
#include <optional>
#include <mutex>
#include "Tile.h"
#include "Utility/mathUtils.h"
#include "Entities/Villager.h"
#include "Utility/Light.h"
#include "Stockpile.h"
#include "DayCycle.h"
#include "Entities/ObjectManager.h"
#include "Entities/Squad.h"
#include "Utility/ItemLocation.h"
#include "Room.h"

#include "World/Chunk.h"
#include "Utility/Pair.h"
#include "World/Colony.h"

#include <entt/entt.hpp>

class Creature;

enum struct PlacementMode {
    SINGLE,
    LINE,
	SQUARE,
    FILLED_SQUARE
};

class World {
private:

    World() = default;
    ~World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    std::atomic<bool> rendered{false};
    std::atomic<bool> currentlyRendering{false};
    std::atomic<int> chunksRendered{0};

    // to be implemented
    static constexpr int CREATURE_CAP = 5000;

    std::unordered_map<std::pair<int, int>, Chunk, pair_hash> chunks;

    LightManager lightManager;
    Colony playerColony;

    // Move this to the temperaturemanager
	std::vector<float> temperatureMap;
    int seed = 0;

    void renderWorld();
    void addCreatures();
public:

    entt::registry registry;

	PlacementMode placementMode = PlacementMode::SQUARE;

    // refactor this stuff
	DayCycle dayCycle;
    ObjectManager objectManager{ 1000, 1000 };
    SquadManager squadManager;

    static World& get();

    [[nodiscard]] LightManager& getLightManager() noexcept { return lightManager; }
    [[nodiscard]] const LightManager& getLightManager() const noexcept { return lightManager; }

    [[nodiscard]] Colony& getColony() noexcept { return playerColony; }
    [[nodiscard]] const Colony& getColony() const noexcept { return playerColony; }

    [[nodiscard]] int getSeed() const { return seed; }

    void initTemperatureMap() {
        int size = calculateMapSize() * 2 + 1;
        temperatureMap.resize(size * size);
        std::fill(temperatureMap.begin(), temperatureMap.end(), 80.f);
	}

    void setTemperatureMap(const std::vector<float>& newMap) {
        temperatureMap = newMap;
	}

    [[nodiscard]] float getTemperatureMapIndex(int x, int y) {
        const int r = calculateMapSize();
        x += r;
        y += r;
        int size = r * 2 + 1;
        if (x < 0 || x >= size || y < 0 || y >= size)
            return 0.5f;
        return temperatureMap[x + y * size];
	}

    Chunk& loadOrGenerateChunk(int x, int y);
    [[nodiscard]] Chunk* getChunk(int x, int y);

    void addTileToMinimap(Chunk& chunk);
    void updateMiniMap();

    [[nodiscard]] auto& getChunks() noexcept { return chunks; }
    [[nodiscard]] const auto& getChunks() const noexcept { return chunks; }

    void generateWorld();

    [[nodiscard]] int getChunksRendered() const { return chunksRendered; }
    [[nodiscard]] bool isCurrentlyRendering() const { return currentlyRendering; };
    [[nodiscard]] bool isRendered() const { return rendered; }
};


extern World& mainWorld;