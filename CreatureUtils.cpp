#include "Pair.h"
#include "Tile.h"

#include <queue>
#include <unordered_set>

std::vector<std::pair<int, int>> getNeighbors(int x, int y) {
    return { {x - 1, y}, {x, y - 1}, {x + 1, y}, {x, y + 1} };
}

std::pair<int, int> findClosestTileItem(const Item& item, int xPos, int yPos) {
    std::queue<std::pair<int, int>> frontier;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    frontier.push({ xPos, yPos });
    visited.insert({ xPos, yPos });

    while (!frontier.empty()) {
        auto current = frontier.front();
        frontier.pop();

        Tile& tile = getTileRef(current.first, current.second);
        if (tile.containsItem(item)) {
            return current;
        }

        for (auto& neighbor : getNeighbors(current.first, current.second)) {
            if (visited.count(neighbor) == 0) {
                visited.insert(neighbor);
                frontier.push(neighbor);
            }
        }
    }
    
    return { xPos, yPos };
}

bool isAtItem(Item item, int x, int y) {


    if (getTileRef(x, y).containsItem(item)) {
        return true;
    }

    for (auto& i : getNeighbors(x, y)) {
        if (getTileRef(i.first, i.second).containsItem(item)) {
            return true;
        }
    }
    return false;
}