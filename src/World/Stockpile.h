#pragma once
#include <unordered_map>
#include <vector>
#include <optional>
#include <utility>
#include <unordered_set>
#include <entt/entt.hpp>

#include "Utility/Pair.h"

class Stockpile {
public:
    Stockpile(std::pair<int, int> loc, int width, int height)
        : location(loc), width(width), height(height) {

    }

    bool operator==(const Stockpile& other) const {
        return this->location.first == other.location.first &&
            this->location.second == other.location.second;
    }

	[[nodiscard]] std::pair<int, int> getLocation() const { return location; }
    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }

    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> getItems() const;
    std::unordered_map<std::string, int> getItemCounts();

    [[nodiscard]] bool atStockpile(int x, int y) const;
    [[nodiscard]] bool claimed(int x, int y) const;

    void releaseTile(int x, int y);
    void addItem(entt::entity obj, int x, int y);
    void placeItem(entt::entity obj, int x, int y);
    void removeItem(int x, int y, entt::entity item);

    [[nodiscard]] entt::entity retrieveItem(int x, int y);

    std::optional<std::pair<int, int>> findOpenSpot();
    std::optional<std::pair<int, int>> findClosestOpenSpot(int x, int y);
    std::optional<std::pair<int, int>> findItemLocation(const std::string& name);
	std::optional<std::pair<int, int>> findUnclaimedItemLocation(const std::string& name);

    void printContents();

private:
    // Top left
    std::pair<int, int> location;
    int width, height;

    // Used to get the item at a given tile
    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> tileItems;

    // Checks claimed tiles
    std::unordered_set<uint64_t> claimedTiles;

    [[nodiscard]] uint64_t getKey(int x, int y) const {
        return (static_cast<uint64_t>(x) << 32) | (static_cast<uint32_t>(y));
    }

    
};