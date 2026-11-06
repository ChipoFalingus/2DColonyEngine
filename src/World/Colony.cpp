#include "World/Colony.h"
#include "Entities/ItemComponents.h"


[[nodiscard]] Stockpile* Colony::atStockpile(int x, int y) {
    for (auto& s : stockpiles) {
        if (s.atStockpile(x, y)) {
            return &s;
        }
    }
    return nullptr;
}

[[nodiscard]] std::optional<std::pair<Stockpile*, std::pair<int, int>>> Colony::findStockpileSpotForItem(const std::string& itemName, int villagerX, int villagerY) {

    Stockpile* bestStockpile = nullptr;
    std::optional<std::pair<int, int>> bestSpot;

    int bestDist = INT_MAX;

    for (auto& s : stockpiles) {
        auto loc = s.getLocation();
        int dx = std::abs(loc.first - villagerX);
        int dy = std::abs(loc.second - villagerY);
        int dist = dx * dx + dy * dy;

        auto spot = s.findClosestOpenSpot(villagerX, villagerY);

        if (dist < bestDist && spot) {
            bestDist = dist;
            bestStockpile = &s;
            bestSpot = spot;
        }
    }

    if (!bestStockpile || !bestSpot) {
        return std::nullopt;
    }

    return std::make_pair(bestStockpile, *bestSpot);
}

// returns both location and the stockpile its found on
[[nodiscard]] std::optional<std::pair<Stockpile*, std::pair<int, int>>> Colony::findItemInAllStockpile(const std::string& name) {
    for (auto& s : stockpiles) {
        auto it = s.findItemLocation(name);
        if (it) {
            return std::make_pair(&s, *it);
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<ItemLocation> Colony::findUnclaimedItemInAllStockpile(const std::string& name, const entt::registry& registry) {
    for (auto& s : stockpiles) {
        auto it = s.findUnclaimedItemLocation(name);
        if (it) {
            auto itemEntity = s.retrieveItem(it->first, it->second);
            if (!registry.try_get<Claimable>(itemEntity)->claimed) {
                ItemLocation itemLocation;
                itemLocation.x = it->first;
                itemLocation.y = it->second;
                itemLocation.item = itemEntity;
                return itemLocation;
            }
        }
    }
    return std::nullopt;
}

void Colony::updateBorders(int x, int y) {
    int dx = x - center.first;
    int dy = y - center.second;

    constexpr int expansionPadding = 10;

    if (dx - expansionPadding < borders[0]) {
        borders[0] = static_cast<int16_t>(dx - expansionPadding);
    }
    if (dx + expansionPadding > borders[1]) {
        borders[1] = static_cast<int16_t>(dx + expansionPadding);
    }
    if (dy - expansionPadding < borders[2]) {
        borders[2] = static_cast<int16_t>(dy - expansionPadding);
    }
    if (dy + expansionPadding > borders[3]) {
        borders[3] = static_cast<int16_t>(dy + expansionPadding);
    }
}