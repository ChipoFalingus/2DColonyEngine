#pragma once

#include <optional>
#include <functional>

#include "Object.h"
#include "ItemLocation.h"


std::optional<ItemLocation> findClosestItemType(int xPos, int yPos, int radius, std::function<bool(const Object& item, int x, int y)> filter);
std::optional<std::vector<ItemLocation>> findAllItemInRange(int xPos, int yPos, int radius, std::function<bool(const Object&, int, int)> filter);
