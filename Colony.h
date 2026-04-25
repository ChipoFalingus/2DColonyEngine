#pragma once
#include "Villager.h"

class Villager;

class Colony {
private:
	std::vector<Villager> villagers;


	std::vector<std::pair<int, int>> stockpiles;

};