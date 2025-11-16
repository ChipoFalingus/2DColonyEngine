#include "Zombie.h"
#include "Tile.h"
#include "Pig.h"
#include "CreatureUtils.h"

Zombie::Zombie(int x, int y)
	: Monster(x, y, L'Z', glm::vec3(0, 255, 0))
{}

float walkSpeed = 0.4f;

void Zombie::doWork() {

	switch (state) {
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
			Creature* closestCreature = findClosestCreatureType<Villager>(xPos, yPos, [this](Creature* c) { return c != this; });
			if (closestCreature && getDistance(closestCreature) < 500.0f) {
				state = monsterState::ATTACKING;
			}
			break;
		}
		case monsterState::ATTACKING:
			if (currentPath.empty()) {
				targetCreature = findClosestCreatureType<Villager>(xPos, yPos, [this](Creature* c) { return c != this; });

				if (currentPath.empty()) {

					targetCreature = findClosestCreatureType<Villager>(xPos, yPos,
						[this](Creature* c) { return c != this; });

					float bestDist = std::numeric_limits<float>::max();
					int bestX = xPos;
					int bestY = yPos;

					for (auto& i : getNeighbors(xPos, yPos)) {
						int dx = i.first - targetCreature->xPos;
						int dy = i.second - targetCreature->yPos;
						float dist = dx * dx + dy * dy;

						if (dist < bestDist && getTileRef(i.first, i.second).walkable) {
							bestDist = dist;
							bestX = i.first;
							bestY = i.second;
						}
					}

					if (bestDist < std::numeric_limits<float>::max()) {
						currentPath.push_back({ bestX, bestY });
					}
					attack();
				}

			}
			break;
	}

	if (!currentPath.empty() && walkSpeed < clock.getElapsedTime().asSeconds()) {
		auto nextStep = currentPath.front();
		currentPath.erase(currentPath.begin());
		xPos = nextStep.first;
		yPos = nextStep.second;
		clock.restart();
	}
}