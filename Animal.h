#pragma once
#include "Creature.h"

#include <vector>

class Animal : public Creature {

public:

	
	bool isHungry = false;
	int hunger = 0;
	const int maxHunger = 100;

	void doWork() override;



	//const int hungerThreshold = 70; // Threshold to start seeking food
	//const int hungerIncreaseRate = 1; // Rate at which hunger increases per work cycle

	Animal(int x, int y, wchar_t displayChar, glm::vec3 color)
		: Creature(x, y, displayChar, color)
	{
	}

};