#pragma once
#include "Item.h"

struct Recipe {

	std::vector<Item> ingredients;
	Item result;
	Item* requiredStation;

	Recipe(const std::vector<Item>& ingreds, const Item& res, Item* station)
		: ingredients(ingreds), result(res), requiredStation(station) {
	}

};


Recipe& getWoodenPlankRecipe();
Recipe& getStoneBrickRecipe();