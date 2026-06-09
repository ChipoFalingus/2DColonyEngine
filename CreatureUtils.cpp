#include "Pair.h"
#include "Tile.h"
#include "CreatureUtils.h"

#include <queue>
#include <unordered_set>
#include <functional>

std::optional<std::pair<int, int>> findClosestTileItem(const std::string name, int xPos, int yPos) {

	int maxRadius = 100;

    std::queue<std::pair<int, int>> frontier;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    frontier.push({ xPos, yPos });
    visited.insert({ xPos, yPos });

    while (!frontier.empty()) {
        auto current = frontier.front();
        frontier.pop();

        int dx = current.first - xPos;
        int dy = current.second - yPos;

        if (dx * dx + dy * dy > maxRadius * maxRadius)
            continue;

        Tile& tile = getTileRef(current.first, current.second);
        if (tile.containsItem(name)) {
            //tile.claimed = true;
            return current;
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


bool isAtItem(const Object& item, int x, int y) {

    if (getTileRef(x, y).containsItem(item.name)) {
        return true;
    }

    for (auto& i : getNeighbors(x, y)) {
        if (getTileRef(i.first, i.second).containsItem(item.name)) {
            return true; // This is the line they guessed
        }
    }
    return false;
}

std::pair<int, int> findBestTemperatureTile(int x, int y, int radius, float preferredTemp) {
    std::pair<int, int> bestTile = { x, y };
    float bestScore = -1.0f;
    for (int dx = -radius; dx <= radius; dx++) {
        for (int dy = -radius; dy <= radius; dy++) {
            int nx = x + dx;
            int ny = y + dy;
            if (getTileRef(nx, ny).walkable) {
                float temp = mainWorld.getTemperatureMapIndex(nx, ny);
                float score = bellCurve(temp, preferredTemp);
                if (score > bestScore) {
                    bestScore = score;
                    bestTile = { nx, ny };
                }
            }
        }
    }

    return bestTile;
}