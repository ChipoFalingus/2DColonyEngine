#pragma once
#include <string>
#include <entt/entt.hpp>


struct FuelBurner {
    float clock;
    int fuel_amount;
};

// Requires FuelBurner component
struct Furnace {
    std::string itemToCook;
    float clock;

    bool occupied = false;
};

class FurnaceManager {
private:
    entt::registry& registry;
public:
    FurnaceManager(entt::registry& reg) : registry(reg) {}

    entt::entity findAvailableFurnace(int x, int y);
    entt::entity findAndClaimFurnace(int x, int y);
    void updateFurnaces(float deltaTime);
};