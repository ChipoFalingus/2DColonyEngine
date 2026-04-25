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
    int width, height;
    std::pair<int, int> location;

    // Used to get the item at a given tile
    std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<Object>>, pair_hash> tileItems;

    // Checks claimed tiles
    std::unordered_set<std::pair<int, int>, pair_hash> claimedTiles;
public:
    Stockpile(std::pair<int, int> loc, int width, int height)
		: location(loc), width(width), height(height) {
    }

	std::pair<int, int> getLocation() const { return location; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool atStockpile(int x, int y) {
        return x >= location.first &&
            x < location.first + width &&
			y >= location.second &&
			y < location.second + height;
    }

    bool claimed(int x, int y) {
        return claimedTiles.find({ x, y }) != claimedTiles.end();
    }

    void claimTile(int x, int y) {
        claimedTiles.insert({ x, y });
    }

    void releaseTile(int x, int y) {
        claimedTiles.erase({ x, y });
    }

    void addItem(std::shared_ptr<Object> obj, int x, int y) {
        if (!obj) {
            return;
        }
        tileItems[{x, y}].push_back(obj);
        claimTile(x, y);
    }

    void removeItem(int x, int y, std::shared_ptr<Object> item) {
        auto it = tileItems.find({ x, y });
        if (it == tileItems.end())
            return;

        auto& vec = it->second;

        vec.erase(
            std::remove(vec.begin(), vec.end(), item),
            vec.end()
        );

        if (vec.empty()) {
            tileItems.erase(it);
            claimedTiles.erase({ x, y });
            std::cout << "Removed item from stockpile at: " << x << "," << y << "\n";
        }

        
    }

    std::optional<std::shared_ptr<Object>> retrieveItem(int x, int y) {
        auto it = tileItems.find({ x, y });
        if (it != tileItems.end()) {
            return it->second[0];
        }
        return std::nullopt;
    }

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

    std::optional<std::pair<int, int>> findItemLocation(const std::string& name) {
        std::cout << "Looking for: " << name << "\n";

        for (auto& [pos, item] : tileItems) {
            if (item[0]->name == name) {
                std::cout << "Found item!\n";
                return pos;
            }
        }
        return std::nullopt;
    }

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