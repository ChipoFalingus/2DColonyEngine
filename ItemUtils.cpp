#include "ItemUtils.h"

#include <queue>
#include <unordered_set>

#include "Pair.h"
#include "Tile.h"

std::optional<ItemLocation> findClosestItemType(int xPos, int yPos, int radius, std::function<bool(const Object&, int, int)> filter) {
    std::queue<std::pair<int, int>> frontier;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    frontier.push({ xPos, yPos });
    visited.insert({ xPos, yPos });
    while (!frontier.empty()) {
        auto current = frontier.front();
        frontier.pop();
        Tile& tile = getTileRef(current.first, current.second);

        int dx = current.first - xPos;
        int dy = current.second - yPos;
        if (dx * dx + dy * dy > radius * radius)
            continue;

        for (const auto& item : tile.items) {
            if (filter(*item, current.first, current.second)) {
                return ItemLocation(current.first, current.second, item);
            }
        }
        for (auto& neighbor : getNeighbors(current.first, current.second)) {
            if (visited.count(neighbor) == 0) {
                visited.insert(neighbor);
                frontier.push(neighbor);
            }
        }
    }

    return std::nullopt;
}

std::optional<std::vector<ItemLocation>> findAllItemInRange(int xPos, int yPos, int radius, std::function<bool(const Object&, int, int)> filter) {
    std::vector<ItemLocation> foundItems;
    for (int x = xPos - radius; x <= xPos + radius; x++) {
        for (int y = yPos - radius; y <= yPos + radius; y++) {
            Tile& tile = getTileRef(x, y);
            for (const auto& item : tile.items) {
                if (filter(*item, x, y)) {
                    foundItems.push_back(ItemLocation(x, y, item));
                }
            }
        }
    }
    if (foundItems.empty()) {
        return std::nullopt;
    }
    return foundItems;
}