#pragma once

#include "Object.h"
#include "Tile.h"
#include "Villager.h"

enum Walkability {
	Open,
	Filtered,
	Blocked
};

class Structure : public Object {
public:
	int health =  100;
	int maxHealth;

	int x, y;

	Walkability walkabilityStatus;
	std::function<bool(Creature*)> filter;

	void takeDamage(int damage) {
		if (health <= 0) return;

		health -= damage;

		if (health <= 0) {
			getTileRef(x, y).removeItem(name);
			getTileRef(x, y).walkable = true;
			std::cout << "Broken" << std::endl;
		}
	}

	virtual bool getWalkability(Creature* c) {
		switch (walkabilityStatus) {
		case Walkability::Open:    return true;
		case Walkability::Blocked:   return false;
		case Walkability::Filtered:  return false;
		}
		return false;
	};
};

class Gate : public Structure {
public:
	Gate() { walkabilityStatus = Walkability::Filtered; }

	bool getWalkability(Creature* c) override {
		switch (walkabilityStatus) {
		case Open: {
			return true;
		}
		case Blocked: {
			return false;
		}
		case Filtered: {
			if (filter) return filter(c);
			else return false;
		}
		}
	}
};