#pragma once
#include <vector>
#include <optional>
#include <mutex>
#include "Tile.h"
#include "mathUtils.h"
#include "Zombie.h"
#include "Villager.h"
#include "Pig.h"
#include "Gun.h"
#include "Light.h"
#include "Stockpile.h"

#include "Chunk.h"
#include "Pair.h"

class Creature;
class Colony;
class Villager;

enum struct PlacementMode {
    SINGLE,
    LINE,
	SQUARE
};

class World {
private:

    World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    std::atomic<bool> rendered = false;
    std::atomic<bool> currentlyRendering = false;
    std::atomic<int> chunksRendered = 0;

    std::vector<Colony> worldColonies;
    std::vector<std::unique_ptr<Creature>> allCreatures;

    std::unordered_map<std::pair<int, int>, Chunk, pair_hash> chunks;

    // Stockpiles need to be in the colony class
    std::vector<Stockpile> stockpiles;
	
    // This needs to be in the colony class too
    std::vector<std::pair<std::shared_ptr<Object>, std::pair<int, int>>> itemsToMove;


    std::vector<float> lightMap;

    void renderWorld();
    void addCreatures();
public:

	PlacementMode placementMode = PlacementMode::LINE;
    
    static World& get();

	const std::vector<Colony>& getColonies() const { return worldColonies; }
	std::vector<std::unique_ptr<Creature>>& getAllCreatures() { return allCreatures; }

    std::vector<Villager*> getAllVillagers();

    void addCreature(std::unique_ptr<Creature> creature) {
        allCreatures.push_back(std::move(creature));
    }

    void removeCreature(Creature* creature) {
        allCreatures.erase(
            std::remove_if(allCreatures.begin(), allCreatures.end(),
                [&](const std::unique_ptr<Creature>& c) {
                    return c.get() == creature;
                }),
            allCreatures.end()
        );
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

        return lightMap[x + y * size];
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

    std::optional<std::pair<std::pair<int, int>, std::shared_ptr<Object>>> findUnclaimedItemInAllStockpile(const std::string& name) {
        for (auto& s : stockpiles) {
            auto it = s.findUnclaimedItemLocation(name);
            if (it) {
                auto itemOpt = s.retrieveItem(it->first, it->second);
                if (itemOpt && !itemOpt->get()->claimed) {
					return std::make_pair(*it, *itemOpt);
                }
            }
        }
        return std::nullopt;
	}

    int countItemInStockpiles(const std::string& item) {
        int count = 0;

        for (auto& s : stockpiles) {
            int w = s.getLocation().first;
            int h = s.getLocation().second;
            for (int i = w; i < s.getWidth() + w; i++) {
                for (int j = h; j < s.getHeight() + h; j++) {
                    if (s.retrieveItem(i, j)->get()->name == item) {
                        count++;
                    }
                }
            }
        }

        return count;
    }

    Chunk& loadOrGenerateChunk(int x, int y);
    void addTileToMinimap(Chunk& chunk);
    Chunk* getChunk(int x, int y);
    void updateMiniMap();

	std::unordered_map<std::pair<int, int>, Chunk, pair_hash>& getChunks() { return chunks; }
    
    void addItemToMove(std::shared_ptr<Object> item, int x, int y) {
        itemsToMove.push_back(std::make_pair(item, std::make_pair(x, y)));
    }

    void removeItemToMove(Object* item, int x, int y) {
        for (auto it = itemsToMove.begin(); it != itemsToMove.end(); ) {
            if (it->first.get() == item &&
                it->second.first == x &&
                it->second.second == y) {

                it = itemsToMove.erase(it);
            }
            else {
                it++;
            }
        }
    }

    std::vector<std::pair<std::shared_ptr<Object>, std::pair<int, int>>>& getItemsToMove() {
        return itemsToMove;
    }

    void generateWorld();

    int getChunksRendered() { return chunksRendered; }
    bool isCurrentlyRendering() const { return currentlyRendering; };
    bool isRendered() const { return rendered; }


};

extern World& mainWorld;