#pragma once
#include "Item.h"
#include "Tool.h"

class Recipe {

public:
	std::unordered_map<std::string, int> ingredients;
	std::string result;
	std::string requiredStation;
	int quantity;
	int time = 0;

    ToolMaterial material = ToolMaterial::NONE;

	Recipe() : quantity(0) {}

    Recipe (std::unordered_map<std::string, int> ingreds, std::string res, std::string station, int quantity)
		: ingredients(ingreds), result(res), requiredStation(station), quantity(quantity) {
	}

};


class RecipeRegistry {
public:
    static RecipeRegistry& getInstance() {
        static RecipeRegistry instance;
        return instance;
    }

    void addRecipe(const Recipe& recipe) {
		std::string key = recipe.result;
        if (recipe.requiredStation != "None") {
			//key += "_" + recipe.requiredStation;
        }
		recipes[key] = recipe;
    }

    Recipe* get(const std::string& name) {
        if (recipes.find(name) == recipes.end()) {
            std::cout << "Recipe not found in registry: " << name << std::endl;
            return nullptr;
        }
        return &recipes.at(name);
    }

    int getSize() {
        return recipes.size();
    }

    std::unordered_map<std::string, Recipe>& getRecipeTable() {
        return recipes;
    }

private:
    std::unordered_map<std::string, Recipe> recipes;
};

void loadRecipes();