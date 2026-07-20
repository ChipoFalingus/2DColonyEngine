#include "Crafting.h"

#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

void loadRecipes() {
	std::ifstream file("Recipes.json");
	json  data;
	file >> data;

	for (auto& i : data.at("recipes")) {


		std::string result = i.at("result").get<std::string>();
		int quantity = i.at("quantity").get<int>();
		std::string bench = i.at("required_bench").get<std::string>();

		std::unordered_map<std::string, int> ingredients;

		for (auto& item : i.at("ingredients").items()) {
			std::string name = item.key();
			int amount = item.value().get<int>();
			ingredients[name] = amount;
		}

		Recipe recipe(
			ingredients,
			result,
			bench,
			quantity
		);

		if (i.contains("time")) {
			recipe.time = i.at("time").get<int>();
		}

		/*if (i.contains("material")) {
			recipe.material = stringToMaterial(i.at("material").get<std::string>());
		}*/

		RecipeRegistry::getInstance().addRecipe(recipe);

		std::cout << "Loaded recipe: " << recipe.result << std::endl;
	}
}