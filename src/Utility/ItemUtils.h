#pragma once

#include <optional>
#include <functional>

#include "Entities/Object.h"
#include "ItemLocation.h"
#include <entt/entt.hpp>


std::optional<ItemLocation> findClosestItemType
(int xPos, int yPos, int radius, std::function<bool(entt::entity, entt::registry&, int, int)> filter);

std::optional<std::vector<ItemLocation>> findAllItemInRange
(int xPos, int yPos, int radius, std::function<bool(entt::entity, entt::registry&, int, int)> filter);
