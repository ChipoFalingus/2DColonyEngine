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
#include "Utility/ItemLocation.h"
#include "Room.h"

#include "World/Chunk.h"
#include "Utility/Pair.h"

#include <entt/entt.hpp>

class Creature;
class Colony;

enum struct PlacementMode {
    SINGLE,
    LINE,
	SQUARE,
    FILLED_SQUARE
};

class World {
private:

    World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    std::atomic<bool> rendered = false;
    std::atomic<bool> currentlyRendering = false;
    std::atomic<int> chunksRendered = 0;

    std::unordered_map<std::pair<int, int>, Chunk, pair_hash> chunks;

    // These need to be in a colony class
    std::vector<Stockpile> stockpiles;
    // ^ make a manager for this later
    std::vector<std::pair<entt::entity, std::pair<int, int>>> itemsToMove;
	RoomManager roomManager;

    // Move this to the lightmanager
    std::vector<float> lightMap;

    // Move this to the temperaturemanager
	std::vector<float> temperatureMap;

    void renderWorld();
    void addCreatures();
public:

    entt::registry registry;

	PlacementMode placementMode = PlacementMode::SQUARE;

	DayCycle dayCycle;
    ObjectManager objectManager;

    static World& get();

    std::vector<entt::entity> getAllVillagers();

	RoomManager& getRoomManager() { return roomManager; }

    void initTemperatureMap() {
        int size = calculateMapSize() * 2 + 1;
        temperatureMap.resize(size * size);
        std::fill(temperatureMap.begin(), temperatureMap.end(), 80.f);
	}

    void setTemperatureMap(const std::vector<float>& newMap) {
        temperatureMap = newMap;
	}

    float getTemperatureMapIndex(int x, int y) {
        x += calculateMapSize();
        y += calculateMapSize();
        int size = calculateMapSize() * 2 + 1;
        if (x < 0 || x >= size || y < 0 || y >= size)
            return 0.5f;
        return temperatureMap[x + y * size];
	}

    void initLightMap() {
        int size = calculateMapSize() * 2 + 1;

        lightMap.resize(size * size);
        std::fill(lightMap.begin(), lightMap.end(), 0.2f);
    }

    void setLightMap(const std::vector<float>& newMap) {
        lightMap = newMap;
    }

    std::vector<float>& getLightMap() {
        return lightMap;
    }

    float getLightMapIndex(int x, int y) {
        x += calculateMapSize();
        y += calculateMapSize();
        int size = calculateMapSize() * 2 + 1;
        if (x < 0 || x >= size || y < 0 || y >= size)
            return 0.2f;

        return std::max(dayCycle.getDaylightFactor(), lightMap[x + y * size]);
    }


    std::vector<Stockpile>& getStockpiles() {
        return stockpiles;
    }

    void addStockpile(const Stockpile& stockpile) {
        stockpiles.push_back(stockpile);
	}

    Stockpile* atStockpile(int x, int y) {
        for (auto& s : stockpiles) {
            if (s.atStockpile(x, y)) {
                return &s;
            }
        }
        return nullptr;
    }

    std::optional<std::pair<Stockpile*, std::pair<int, int>>> findStockpileSpotForItem(const std::string& itemName, int villagerX, int villagerY) {

        Stockpile* bestStockpile = nullptr;
        std::optional<std::pair<int, int>> bestSpot;

        int bestDist = INT_MAX;

        for (auto& s : stockpiles) {
            auto loc = s.getLocation();
            int dx = std::abs(loc.first - villagerX);
            int dy = std::abs(loc.second - villagerY);
            int dist = dx * dx + dy * dy;

            auto spot = s.findOpenSpot();

            if (dist < bestDist && spot) {
                bestDist = dist;
                bestStockpile = &s;
                bestSpot = spot;
            }
        }

        if (!bestStockpile || !bestSpot) {
            return std::nullopt;
        }

        return std::make_pair(bestStockpile, *bestSpot);
    }

    // returns both location and the stockpile its found on
    std::optional<std::pair<Stockpile*, std::pair<int, int>>> findItemInAllStockpile(const std::string& name) {
        for (auto& s : stockpiles) {
            auto it = s.findItemLocation(name);
            if (it) {
                return std::make_pair(&s, *it);
            }
        }
        return std::nullopt;
    }

    std::optional<ItemLocation> findUnclaimedItemInAllStockpile(const std::string& name) {
        for (auto& s : stockpiles) {
            auto it = s.findUnclaimedItemLocation(name);
            if (it) {
                auto itemEntity = s.retrieveItem(it->first, it->second);
                if (!registry.try_get<Claimable>(itemEntity)->claimed) {
                    ItemLocation itemLocation;
                    itemLocation.x = it->first;
                    itemLocation.y = it->second;
                    itemLocation.item = itemEntity;
					return itemLocation;
                }
            }
        }
        return std::nullopt;
	}

    Chunk& loadOrGenerateChunk(int x, int y);
    void addTileToMinimap(Chunk& chunk);
    Chunk* getChunk(int x, int y);
    void updateMiniMap();

	std::unordered_map<std::pair<int, int>, Chunk, pair_hash>& getChunks() { return chunks; }
    
    void addItemToMove(entt::entity item, int x, int y) {
        itemsToMove.push_back(std::make_pair(item, std::make_pair(x, y)));
    }

    std::vector<std::pair<entt::entity, std::pair<int, int>>>& getItemsToMove() {
        return itemsToMove;
    }

    void generateWorld();

    int getChunksRendered() { return chunksRendered; }
    bool isCurrentlyRendering() const { return currentlyRendering; };
    bool isRendered() const { return rendered; }


};

extern World& mainWorld;