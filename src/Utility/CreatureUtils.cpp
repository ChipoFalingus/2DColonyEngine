#include "Pair.h"
#include "World/Tile.h"
#include "CreatureUtils.h"

#include <queue>
#include <unordered_set>
#include <functional>

std::pair<int, int> findBestTemperatureTile(int x, int y, int radius, float preferredTemp) {
    std::pair<int, int> bestTile = { x, y };
    float bestScore = 0.f;

    for (int dx = -radius; dx <= radius; dx++) {
        for (int dy = -radius; dy <= radius; dy++) {
            int nx = x + dx;
            int ny = y + dy;
			Tile& tile = getTileRef(nx, ny);

            if (!tile.walkable) continue;

            float comfortScore = bellCurve(mainWorld.getTemperatureMapIndex(nx, ny), preferredTemp, 8);
            float distance = std::abs(dx) + std::abs(dy);

            float distancePenalty = distance * 0.01f;
            float finalScore = comfortScore - distancePenalty;

            if (finalScore > bestScore) {
                bestScore = finalScore;
                bestTile = { nx, ny };
            }
		}
    }

    return bestTile;
}