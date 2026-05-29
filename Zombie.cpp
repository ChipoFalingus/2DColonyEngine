#include "Zombie.h"
#include "Tile.h"
#include "Pig.h"
#include "CreatureUtils.h"

Zombie::Zombie(int x, int y)
	: Monster(x, y, L'Z', glm::vec3(0, 255, 0))
{
	speed = 0.2f;
}

void Zombie::doWork() {

	if (!currentPath.empty() && speed < clock.getElapsedTime().asSeconds()) {
		auto nextStep = currentPath.front();
		currentPath.erase(currentPath.begin());
		xPos = nextStep.first;
		yPos = nextStep.second;
		clock.restart();
	}
}