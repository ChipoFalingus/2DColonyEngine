#include "Entities/Furnace.h"

#include "Utility/ItemUtils.h"
#include "Utility/Clock.h"
#include "Entities/ObjectManager.h"
#include "World/Tile.h"

entt::entity FurnaceManager::findAvailableFurnace(int x, int y) {
    auto loc = findClosestItemType(x, y, 100, [](entt::entity entity, entt::registry& reg, int x, int y) {
        auto* furnace = reg.try_get<Furnace>(entity);
        auto* claimed = reg.try_get<Claimable>(entity);

        return claimed && !claimed->claimed && furnace /*&& !furnace->occupied*/;
        });

    return (loc.has_value()) ? loc->item : entt::null;
}

entt::entity FurnaceManager::findAndClaimFurnace(int x, int y) {
    entt::entity furnace = findAvailableFurnace(x, y);
    if (furnace == entt::null) return furnace;

    registry.get<Claimable>(furnace).claimed = true;
    return furnace;
}

void FurnaceManager::updateFurnaces(float deltaTime) {
    auto view = registry.view<Position, FuelBurner, Furnace>();

    for (auto [entity, pos, burner, furnace] : view.each()) {
        burner.fuel_amount = 100;

        if (furnace.itemToCook.empty() || burner.fuel_amount <= 0) {
            //registry.remove<LightEmitter, HeatEmitter>(entity);
            furnace.occupied = false;
            continue;
        }

        furnace.occupied = true;
        burner.clock += Clock::deltaTime;

        if (burner.clock >= 1.0f) {
            burner.fuel_amount--;
            burner.clock = 0.0f;
        }

        if (burner.fuel_amount > 0) {
            furnace.clock += Clock::deltaTime;

            if (furnace.clock > 10.0f) {
                getTileRef(pos.x, pos.y).addObject(pos.x, pos.y, furnace.itemToCook, true);
                furnace.itemToCook.clear();
                furnace.clock = 0.0f;
                registry.get<Claimable>(entity).claimed = false;
            }
        }
    }
}