#pragma once
#include <vector>
#include <utility>

#include "Tile.h"

enum SedimentState {
    DEPOSIT,
    TRANSPORT,
    ERODE
};

class WaterSimulation {
private:
    std::vector<std::pair<int, int>> basins;
    std::vector<std::pair<int, int>> newWater;

    SedimentState hjulstromDiagram(float flowVelocity, float grainSize) const;
public:
    void addBasin(int x, int y);
    void simulateWaterTile(Tile& tile, int x, int y);
    std::vector<std::pair<int, int>> getWater() const;
};






