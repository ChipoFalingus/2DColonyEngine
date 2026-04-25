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

	/*switch (state) {
		case monsterState::IDLE:
			state = monsterState::WANDERING;
			break;

		case monsterState::WANDERING: {
			int dir = getRandomInt(0, 3);
			int newX = xPos;
			int newY = yPos;
			switch (dir) {
				case 0: newX += 1; break;
				case 1: newX -= 1; break;
				case 2: newY += 1; break;
				case 3: newY -= 1; break;
			}
			if (getTileRef(newX, newY).walkable) {
				currentPath.push_back({ newX, newY });
			}
			Creature* closestCreature = findClosestCreatureType<Villager>(xPos, yPos, 100, [this](Creature* c) { return c != this; });
			if (closestCreature) {
				state = monsterState::ATTACKING;
			}
			break;
		}
		case monsterState::ATTACKING:

			if (!targetCreature) {
				targetCreature = findClosestCreatureType<Villager>(
					xPos, yPos, 100,
					[this](Creature* c) {
						return c != static_cast<Creature*>(this);
					}
				);
			}

			if (targetCreature && targetCreature->dead) targetCreature = nullptr;
			if (!targetCreature) {
				state = monsterState::WANDERING;
				break;

			}
			int targetTileX = targetCreature->xPos;
			int targetTileY = targetCreature->yPos;

			if (std::abs(xPos - targetTileX) + std::abs(yPos - targetTileY) <= 1) {
				if (attackClock.getElapsedTime().asSeconds() > 0.5f) {
					attackClock.restart();
					targetCreature->takeDamage(1, this);
				}
			}

			if (currentPath.empty() ||
				lastTargetTileX != targetTileX ||
				lastTargetTileY != targetTileY)
			{
				currentPath = findPath(xPos, yPos, { targetTileX, targetTileY });

				lastTargetTileX = targetTileX;
				lastTargetTileY = targetTileY;
			}

			break;
	}*/

	if (!currentPath.empty() && speed < clock.getElapsedTime().asSeconds()) {
		auto nextStep = currentPath.front();
		currentPath.erase(currentPath.begin());
		xPos = nextStep.first;
		yPos = nextStep.second;
		clock.restart();
	}
}