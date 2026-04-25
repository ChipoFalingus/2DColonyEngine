#pragma once
#include "Monster.h"
#include "Tile.h"

class Zombie : public Monster {
private:
	int lastTargetTileX = -1;
	int lastTargetTileY = -1;
public:
	Zombie(int x, int y);
	void doWork() override;
};