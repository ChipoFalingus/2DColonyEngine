#include "WaterSimulation.h"
#include "Tile.h"

void WaterSimulation::addBasin(int x, int y) {
    auto target = std::make_pair(x, y);
    auto it = std::find(basins.begin(), basins.end(), target);

    if (it == basins.end()) {
        std::cout << "Basin low at " << x << ", " << y << std::endl;
        basins.push_back(target);
    }
}

SedimentState WaterSimulation::hjulstromDiagram(float flowVelocity, float grainSize) const {
    if (flowVelocity < 0.1f) {
        return DEPOSIT;
        std::cout << "Deposit" << std::endl;
    }
    else if (flowVelocity < 1.0f) {
        if (grainSize < 0.05f) {
            return TRANSPORT;
            std::cout << "Transport" << std::endl;
        }
        else {
            return DEPOSIT;
            std::cout << "Deposit" << std::endl;
        }
    }
    else {
        if (grainSize < 0.1f) {
            return ERODE;
            std::cout << "Erode" << std::endl;
        }
        else {
            return TRANSPORT;
            std::cout << "Transport" << std::endl;
        }
    }

}


//void WaterSimulation::simulateWaterTile(Tile& tile, int x, int y) {
//
//    for (auto& i : basins) {
//        if (i.first == x && i.second == y) {
//            if (tile.water < 500.0f) {
//                tile.water += 200.5f;
//            }
//        }
//    }
//
//    float grainSize = 0.002f; // In meters
//
//    float surface = tile.altitude + tile.water;
//
//    for (auto& n : tile.getNeighbors()) {
//        Tile& other = getTileRef(n.first, n.second);
//
//        float otherSurface = other.altitude + other.water;
//
//        if (surface > otherSurface) {
//
//            float flow = (surface - otherSurface) * 0.5f;
//
//            flow = std::min(flow, tile.water);
//            tile.flow = flow;
//
//            tile.water -= flow;
//            other.water += flow;
//
//            surface = tile.altitude + tile.water;
//
//            // Hjulstrom's Graph implementation
//
//            float flowVelocity = flow * 10.0f;
//
//            float erosionStrength = 0.0005f;
//            float depositionStrength = 0.0003f;
//
//            SedimentState state = hjulstromDiagram(flowVelocity, grainSize);
//
//            if (state == ERODE) {
//                tile.altitude -= flow * erosionStrength;
//            }
//            else if (state == DEPOSIT) {
//                other.altitude += flow * depositionStrength;
//            }
//
//            if (flow >= 1.0f) {
//                other.items.clear();
//            }
//        }
//    }
//}

std::vector<std::pair<int, int>> WaterSimulation::getWater() const {
    return newWater;
}