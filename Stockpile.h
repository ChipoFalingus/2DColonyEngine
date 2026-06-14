#pragma once
#include <unordered_map>
#include <vector>
#include <optional>
#include <utility>
#include <unordered_set>
#include "Item.h"
#include "Pair.h"

class Stockpile {
private:

    std::unordered_map<Type, bool> filter;

    int width, height;

    // Top left
    std::pair<int, int> location;

    // Used to get the item at a given tile
    std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<Object>>, pair_hash> tileItems;

    // Checks claimed tiles
    std::unordered_set<std::pair<int, int>, pair_hash> claimedTiles;
public:
    Stockpile(std::pair<int, int> loc, int width, int height)
        : location(loc), width(width), height(height) {

        filter.insert({ Type::Item, true });
		filter.insert({ Type::Tool, true });
		filter.insert({ Type::Food, true });
		filter.insert({ Type::Crop, true });
		filter.insert({ Type::Foliage_Crop, true });
		filter.insert({ Type::Heat_Emitter, true });
		filter.insert({ Type::Gun, true });
		filter.insert({ Type::Bench, true });
		filter.insert({ Type::Structure, true });
		filter.insert({ Type::Furniture, true });

    }

    std::pair<int, int> getLocation() const { return location; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<Object>>, pair_hash> getItems() {
        return tileItems;
    }

	std::unordered_map<Type, bool> getFilter() const { return filter; }

    std::unordered_map<std::string, int> getFilteredItemCounts(Type type) {
        std::unordered_map<std::string, int> result;
        for (auto& [loc, f] : tileItems) {
            for (auto& item : f) {
                if (item && item->type == type) {
                    result[item->name]++;
                }
            }
        }
		return result;
    }

	std::unordered_map<std::string, int> getItemCounts() { 
        std::unordered_map<std::string, int> result;
        for (auto& [loc, f] : tileItems) {
            for (auto& item : f) {
                if (item) {
                    result[item->name]++;
                }
            }
        }
		return result;
    }

	// Checks if the given coordinates are within the bounds of the stockpile
    bool atStockpile(int x, int y) {
        return x >= location.first &&
            x < location.first + width &&
			y >= location.second &&
			y < location.second + height;
    }

	// Checks if the tile at the given coordinates is claimed
    bool claimed(int x, int y) {
        return claimedTiles.find({ x, y }) != claimedTiles.end();
    }

	// Unclaims the tile at the given coordinates, allowing it to be used again
    void releaseTile(int x, int y) {
        claimedTiles.erase({ x, y });
    }

	// Adds an item to the given location. The tile is claimed if it isn't already, and the item is added to the stockpile's item list
    void addItem(std::shared_ptr<Object> obj, int x, int y) {
        if (!obj) {
            return;
        }
        claimedTiles.insert({ x, y });
    }

    void placeItem(std::shared_ptr<Object> obj, int x, int y) {
        if (!obj) {
			std::cout << "Error: Tried to place null item in stockpile\n";
            return;
        }
        std::cout << "Placing item in stockpile at: " << x << "," << y << std::endl;
        tileItems[{ x, y }].push_back(obj);
    }

	// Removes the given item from the given location. If there are no more items at that location, it is unclaimed and removed from the stockpile's item list
    void removeItem(int x, int y, std::shared_ptr<Object> item) {
        auto it = tileItems.find({ x, y });
        if (it == tileItems.end())
            return;

		std::cout << "Removing " << item->name << " from stockpile at: " << x << "," << y << std::endl;
        auto& vec = it->second;

        vec.erase(
            std::remove(vec.begin(), vec.end(), item),
            vec.end()
        );


        if (vec.empty()) {
            std::cout << "Removed " << item->name << " from stockpile at: " << x << "," << y << "\n";
        }
        else {
			std::cout << "Removed " << item->name << " from stockpile at: " << x << "," << y << ", but there are still " << vec.size() << " items there\n";
        }
        tileItems.erase(it);
        claimedTiles.erase({ x, y });
    }

	// Returns the item at a given location, if it exists
    std::optional<std::shared_ptr<Object>> retrieveItem(int x, int y) {
        auto it = tileItems.find({ x, y });
        if (it != tileItems.end()) {
            return it->second[0];
        }
        return std::nullopt;
    }

	// Finds the first open spot in the stockpile and returns its coordinates
    std::optional<std::pair<int, int>> findOpenSpot() {
        for (int x = location.first; x < location.first + width; x++) {
            for (int y = location.second; y < location.second + height; y++) {
                if (!claimed(x, y)) {
                    std::cout << "Found open spot at: " << x << "," << y << "\n";

                    return std::make_pair(x, y);
                }
            }
		}
		return std::nullopt;
	}

	// Finds the first item with the given name and returns its coordinates
    std::optional<std::pair<int, int>> findItemLocation(const std::string& name) {
        for (auto& [pos, item] : tileItems) {
            if (item[0]->name == name) {
                std::cout << "Found item!\n";
                return pos;
            }
        }
        return std::nullopt;
    }

	// Finds the first unclaimed item with the given name and returns its coordinates
    std::optional<std::pair<int, int>> findUnclaimedItemLocation(const std::string& name) {
        std::cout << "Looking for: " << name << "\n";

        for (auto& [pos, item] : tileItems) {
            if (item[0]->name == name && !item[0]->claimed) {
                std::cout << "Found item!\n";
                return pos;
            }
        }
        return std::nullopt;
    }

    void claimItem(int x, int y) {

    }

    void printContents() {
        std::cout << "Stockpile at (" << location.first << "," << location.second << ") contains:" << std::endl;
        for (auto& [pos, items] : tileItems) {
            for (auto& item : items) {
                if (item) {
                    std::cout << "- " << item->name << " at (" << pos.first << "," << pos.second << ")" << std::endl;
                }
            }
        }
	}
};