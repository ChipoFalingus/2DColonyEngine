#include "Crafting.h"
#include "Item.h"

//Recipe woodenPlankRecipe(
//	std::vector<Item>{ wood },
//	Item("Wooden Plank", L'=', sf::Color(139, 69, 19)),
//	&CarpentryBench
//);

Recipe& getWoodenPlankRecipe() {
	Item wood = *ItemRegistry::getInstance().get("Wood");
	Item rock = *ItemRegistry::getInstance().get("Rock");
	Item tree = *ItemRegistry::getInstance().get("Oak Tree");
	Item flower = *ItemRegistry::getInstance().get("Flower");

	Item* carpentryBench = ItemRegistry::getInstance().get("Carpentry Bench");

	static Recipe r(
		std::vector<Item>{ rock, rock, tree, tree, tree, tree, tree, flower, flower },
		Item("Wooden Plank", L'=', sf::Color(139, 69, 19)),
		carpentryBench
	);
	return r;
}