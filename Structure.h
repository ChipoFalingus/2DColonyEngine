#pragma once

#include "Object.h"
#include "Tile.h"

class Structure : public Object {
public:
	int health =  100;
	int maxHealth;

	int x, y;

	//Structure(int maxHealth) : Object(), health(maxHealth), maxHealth(maxHealth) {}

	void takeDamage(int damage) {
		if (health <= 0) return;

		health -= damage;
		std::cout << "Structure took " << damage << " damage. Health: " << health << std::endl;

		if (health <= 0) {
			getTileRef(x, y).removeItem(name);
			getTileRef(x, y).walkable = true;
			std::cout << "Broken" << std::endl;
		}
	}
};


class Gate : public Object {
public:
	int health = 100;
	int maxHealth;

	int x, y;

	bool getWalkability(Creature* c) {
		if (dynamic_cast<Villager*>(c)) {
			return true;
		}

		return false;
	}
};