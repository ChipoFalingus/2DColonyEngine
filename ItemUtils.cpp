#include "ItemUtils.h"

#include <queue>
#include <unordered_set>

#include "Pair.h"
#include "Tile.h"

std::optional<ItemLocation> findClosestItemType(int xPos, int yPos, int radius, std::function<bool(const Object& item)> filter) {
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
            if (filter(*item)) {
                return ItemLocation(current.first, current.second, item.get());
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