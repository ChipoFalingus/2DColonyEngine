#pragma once
#include <unordered_map>
#include <vector>
#include <optional>
#include <utility>
#include <unordered_set>
#include <entt/entt.hpp>

#include "Pair.h"

class Stockpile {
private:

    int width, height;

    // Top left
    std::pair<int, int> location;

    // Used to get the item at a given tile
    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> tileItems;

    // Checks claimed tiles
    std::unordered_set<std::pair<int, int>, pair_hash> claimedTiles;
public:
    Stockpile(std::pair<int, int> loc, int width, int height)
        : location(loc), width(width), height(height) {

    }

    bool operator==(const Stockpile& other) const {
        return this->location.first == other.location.first &&
            this->location.second == other.location.second;
    }

	std::pair<int, int> getLocation() const { return location; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> getItems();
    std::unordered_map<std::string, int> getItemCounts();

    bool atStockpile(int x, int y) const;
    bool claimed(int x, int y) const;

    void releaseTile(int x, int y);
    void addItem(entt::entity obj, int x, int y);
    void placeItem(entt::entity obj, int x, int y);
    void removeItem(int x, int y, entt::entity item);

    entt::entity retrieveItem(int x, int y);

    std::optional<std::pair<int, int>> findOpenSpot();
    std::optional<std::pair<int, int>> findItemLocation(const std::string& name);
	std::optional<std::pair<int, int>> findUnclaimedItemLocation(const std::string& name);

    void printContents();

    
};