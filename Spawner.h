#pragma once
#include "Object.h"
#include "Creature.h"

#include "Zombie.h"

#include "mathUtils.h"

#include "World.h"
#include "Squad.h"

class Spawner : public Object {
public:
	Creature* spawnCreature;
	std::unique_ptr<Squad> squad = std::make_unique<Squad>();

	float cooldown;
	int x, y;
	int radius;

	sf::Clock spawnClock;

	void spawn() {
		if (spawnClock.getElapsedTime().asSeconds() >= cooldown) {
			// Spawn logic here
			spawnClock.restart();

			int xOffset = getRandomInt(x - radius, x + radius);
			int yOffset = getRandomInt(y - radius, y + radius);

			auto v = std::make_unique<Zombie>(xOffset, yOffset);

			squad->addMember(v.get());
			mainWorld.getAllCreatures().push_back(std::move(v));
		}
	}

	void updateSquad() {
		squad->update();
	}

	void update() {
		spawn();
		if (squad) updateSquad();
	}
};