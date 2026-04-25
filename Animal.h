#pragma once
#include "Creature.h"

#include <vector>

class Animal : public Creature {

public:

	
	std::string name;
	void doWork() override;


	Animal(int x, int y, wchar_t displayChar, glm::vec3 color)
		: Creature(x, y, displayChar, color)
	{
	}

};