#include "ItemUtils.h"
#include "World/World.h"

#include <queue>
#include <unordered_set>

#include "Pair.h"
#include "World/Tile.h"
#include "Entities/ItemComponents.h"

std::optional<ItemLocation> findClosestItemType(int xPos, int yPos, int radius, std::function<bool(entt::entity, entt::registry&, int, int)> filter) {
    auto& registry = mainWorld.registry;

    auto view = registry.view<Position>();

    entt::entity bestEntity = entt::null;
    float closestDistSq = static_cast<float>(radius * radius);
    int bestX = -1;
    int bestY = -1;

    for (auto [entity, pos] : view.each()) {
        int dx = pos.x - xPos;
        int dy = pos.y - yPos;
        float distSq = static_cast<float>(dx * dx + dy * dy);

        if (distSq > closestDistSq) continue;

        if (filter(entity, registry, pos.x, pos.y)) {
            closestDistSq = distSq;
            bestEntity = entity;
            bestX = pos.x;
            bestY = pos.y;
        }
    }

    if (bestEntity != entt::null) {
        return ItemLocation(bestX, bestY, bestEntity);
    }

    return std::nullopt;
}

std::optional<std::vector<ItemLocation>> findAllItemInRange(int xPos, int yPos, int radius, std::function<bool(entt::entity, entt::registry&, int, int)> filter) {
        std::vector<ItemLocation> foundItems;
        auto& registry = mainWorld.registry;

        int minX = xPos - radius;
        int maxX = xPos + radius;
        int minY = yPos - radius;
        int maxY = yPos + radius;
        int radiusSq = radius * radius;

        auto view = registry.view<Position>();

        for (auto entity : view) {
            const auto& pos = view.get<Position>(entity);

            if (pos.x >= minX && pos.x <= maxX && pos.y >= minY && pos.y <= maxY) {

                int dx = pos.x - xPos;
                int dy = pos.y - yPos;
                if (dx * dx + dy * dy <= radiusSq) {

                    if (filter(entity, registry, pos.x, pos.y)) {
                        foundItems.push_back(ItemLocation(pos.x, pos.y, entity));
                    }
                }
            }
        }

        if (foundItems.empty()) {
            return std::nullopt;
        }
        return foundItems;
    }