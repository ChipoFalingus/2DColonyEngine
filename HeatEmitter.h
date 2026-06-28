#pragma once

#include "Object.h"

#include "Crafting.h"
#include "Tile.h"
#include "Game.h"

class HeatEmitter : public Object {
public:
    int x = 0, y = 0;
    float heatIntensity = 0.0f;
    float lightIntensity = 50.0f;
    float heatRadius = 3.0f;
    float fuelAmount = 9999.0f;

    bool on = false;
    bool addedFuelJob = false;

    bool autoRefuel = true;


    HeatEmitter() : Object() {}

    void update() {
        if (!on && fuelAmount <= 0.0f) return;

        fuelAmount -= Clock::deltaTime;

        if (fuelAmount <= 0.0f && on) {
            fuelAmount = 0.0f; 
            disable();
        }
        else if (fuelAmount > 0.0f && !on) {
            enable();
        }
    }

    void enable() {
        if (on) return;
        on = true;
        Tile& tile = getTileRef(x, y);
        tile.setAnimType(FIRE);

        Game::getInstance().getLightManager().addLight(glm::vec2(x, y), glm::vec3(255, 140, 0), 100.0f, lightIntensity, -1.0f);
        Game::getInstance().getHeatManager().addHeatSource(x, y, heatIntensity, heatRadius);
    }

    void disable() {
        if (!on) return;
        on = false;
        getTileRef(x, y).setAnimType(NONE);

        Game::getInstance().getLightManager().removeLight(x, y);
        Game::getInstance().getHeatManager().removeHeatSource(x, y);
    }

    void addFuel(float amount) {
        fuelAmount += amount;
        if (fuelAmount > 0.0f && !on) {
            enable();
        }
    }
};

class Furnace : public HeatEmitter {
public:

	std::shared_ptr<Object> input;

	bool stopped = false;

    float clock = 0.0f;

	Recipe* currentRecipe = nullptr;


    void addInput(std::shared_ptr<Object> item) {
        input = item;
        currentRecipe = RecipeRegistry::getInstance().get("Iron Bar_Furnace");
        clock = 0.0f;
		enable();
        
	}

    void cook() {

        if (!currentRecipe || !input) return;

        if (fuelAmount <= 0.0f) {
            fuelAmount = 0.0f;
            std::cout << "Not enough fuel to cook!" << std::endl;
            disable();
            return;
        }

        clock += Clock::deltaTime;
        fuelAmount -= Clock::deltaTime;

        if (clock >= currentRecipe->time) {
            auto resultItem = ObjectRegistry::getInstance().get(currentRecipe->result);
            mainWorld.addItemToMove(resultItem, x, y);
            getTileRef(x, y).addObject(resultItem);

            clock = 0.0f;
            input = nullptr;
            currentRecipe = nullptr;

            disable();
            std::cout << "Furnace finished smelting!" << std::endl;
        }
    }
};