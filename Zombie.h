#pragma once
#include "Monster.h"
#include "Tile.h"

class Zombie : public Monster {

public:
	Zombie(int x, int y);
	void doWork() override;
};