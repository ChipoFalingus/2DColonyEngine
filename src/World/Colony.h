#pragma once
#include "Stockpile.h"
#include "Room.h"

#include "Utility/ItemLocation.h"

class Colony {

private:
    std::vector<int> retreatMap;
	std::vector<Stockpile> stockpiles;
	RoomManager roomManager;

    std::pair<int, int> center{ 0, 0 };

    // additional ranges where jobs can be done
    uint16_t job_range = 50;

    // top, bottom, left, right
    std::array<int16_t, 4> borders = { -25, 25, -25, 25 };

public:

    Colony() {
        resizeRetreatMap();
    };

    [[nodiscard]] RoomManager& getRoomManager() noexcept { return roomManager; }
	[[nodiscard]] const RoomManager& getRoomManager() const noexcept { return roomManager; }

    [[nodiscard]] const std::vector<Stockpile>& getStockpiles() const noexcept { return stockpiles; }

    [[nodiscard]] const std::array<int16_t, 4> getColonyBorders() const noexcept { return borders; }

    [[nodiscard]] const std::pair<int, int> getCenter() const noexcept { return center; }

    void addStockpile(const Stockpile& stockpile) {
        stockpiles.push_back(stockpile);
    }

    [[nodiscard]] Stockpile* atStockpile(int x, int y);

    [[nodiscard]] std::optional<std::pair<Stockpile*, std::pair<int, int>>> findStockpileSpotForItem(int villagerX, int villagerY);

    // returns both location and the stockpile its found on
    [[nodiscard]] std::optional<std::pair<Stockpile*, std::pair<int, int>>> findItemInAllStockpile(const std::string& name);

    [[nodiscard]] std::optional<ItemLocation> findUnclaimedItemInAllStockpile(const std::string& name, const entt::registry& registry);

    [[nodiscard]] bool isInsideColony(int x, int y, int padding = 0) const;

    void resizeRetreatMap();

    [[nodiscard]] int getRetreatValue(int x, int y);

    // takes in where new thing was built
    void updateBorders(int x, int y);
};