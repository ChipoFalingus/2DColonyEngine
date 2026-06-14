#pragma once
#include <vector>

#include "World.h"

struct HeatSource {
	int x, y;
	float intensity;
	float radius;
};

class HeatManager {
private:
	std::vector<HeatSource> heatSources;

public:

	void addHeatSource(int x, int y, float intensity, float radius) {
		heatSources.push_back({ x, y, intensity, radius });
	}

    void removeHeatSource(int x, int y) {
        heatSources.erase(std::remove_if(heatSources.begin(), heatSources.end(),
            [x, y](const HeatSource& source) { return source.x == x && source.y == y; }),
            heatSources.end());
	}

	std::vector<float> calculateHeatMap(int mapSize) {
        int gridSize = mapSize * 2 + 1;
        std::vector<float> heatMap(gridSize * gridSize, mainWorld.dayCycle.getTemperatureFactor());

        for (const auto& source : heatSources) {
            int localSourceX = source.x + mapSize;
            int localSourceY = source.y + mapSize;

            int startX = std::max(0, localSourceX - static_cast<int>(source.radius));
            int endX = std::min(gridSize - 1, localSourceX + static_cast<int>(source.radius));
            int startY = std::max(0, localSourceY - static_cast<int>(source.radius));
            int endY = std::min(gridSize - 1, localSourceY + static_cast<int>(source.radius));

            for (int y = startY; y <= endY; y++) {
                for (int x = startX; x <= endX; x++) {
                    float dx = x - localSourceX;
                    float dy = y - localSourceY;
                    float distSq = dx * dx + dy * dy;
                    float radiusSq = source.radius * source.radius;

                    if (distSq < radiusSq) {
                        float heatContribution = source.intensity * (1.0f - (distSq / radiusSq));

                        heatMap[x + y * gridSize] += heatContribution;
                    }
                }
            }
        }
        return heatMap;
    }

};