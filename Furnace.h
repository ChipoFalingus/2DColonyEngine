#pragma once

#include "Object.h"
#include <SFML/System/Clock.hpp>

#include "Crafting.h"
#include "Tile.h"
#include "Game.h"

class Furnace : public Object {
public:

	std::shared_ptr<Object> input;

	int fuelAmount = 9999;

    float progress = 0.0f;

	float remainingCookTime = 0.0f;

	bool stopped = false;

    int x, y;

	Recipe* currentRecipe = nullptr;

	Furnace() : Object() {}


    void addInput(std::shared_ptr<Object> item) {
        input = item;
        currentRecipe = RecipeRegistry::getInstance().get("Iron Bar_Furnace");
        progress = 0.0f;

        addLight();
	}

    void addLight() {
		Game::getInstance().getLightManager().addLight(glm::vec2(x,y),glm::vec3(255, 140, 0), 100.0f, 0.5f, -1.0f);
	}

    void cook(float dt) {

        if (!currentRecipe) {
            return;
        }

        if (!input) {
            return;
        }

        if (fuelAmount > 0) {
            stopped = false;

            fuelAmount -= dt;
            progress += dt;

            if (progress >= currentRecipe->time) {
                finish();
            }
        }
        else {
            if (!stopped) {
                stopped = true;
                std::cout << "Not enough fuel to cook!" << std::endl;
            }
        }
    }

	void addFuel(std::shared_ptr<Object> fuel) {
		// fuelAmount += fuel->fuelValue;
	}

    void finish() {
		auto i = ObjectRegistry::getInstance().get(currentRecipe->result);
        mainWorld.addItemToMove(i, x, y);
		getTileRef(x, y).addObject(i); // Replace with actual furnace tile coordinates
        input = nullptr;
        currentRecipe = nullptr;
		progress = 0.0f;
    }   
};