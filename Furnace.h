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

	bool stopped = false;
    int x, y;

    float clock = 0.0f;

	Recipe* currentRecipe = nullptr;

	Furnace() : Object() {}


    void addInput(std::shared_ptr<Object> item) {
        input = item;
        currentRecipe = RecipeRegistry::getInstance().get("Iron Bar_Furnace");
        clock = 0.0f;
        addLight();
	}

    void addLight() {
		Game::getInstance().getLightManager().addLight(glm::vec2(x,y),glm::vec3(255, 140, 0), 100.0f, 1.f, currentRecipe->time);
        std::vector<float> map = Game::getInstance().getLightManager().BFSLight();
        mainWorld.setLightMap(map);
	}

    void cook() {

		clock += Clock::deltaTime;

        if (!currentRecipe) {
            return;
        }

        if (clock < currentRecipe->time) {
            return;
        }

        if (!input) {
            return;
        }

        if (fuelAmount > 0) {
            stopped = false;

            //fuelAmount -= dt;

            if (clock >= currentRecipe->time) {
                clock = 0.0f;
                auto i = ObjectRegistry::getInstance().get(currentRecipe->result);
                mainWorld.addItemToMove(i, x, y);
                getTileRef(x, y).addObject(i);
                input = nullptr;
                currentRecipe = nullptr;
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
        //fuelAmount += fuel->fuelValue;

	}
};