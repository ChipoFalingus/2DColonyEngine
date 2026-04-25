#pragma once
#include "Item.h"

class Gun : public Object {
public:

	int maxAmmo;
	int currentAmmo;
	int damage;
	float range;
	bool fired = false;

	float attackCooldown; //in seconds
	sf::Clock shootClock;

	Gun() : Object(), maxAmmo(0), damage(0), range(0.0f), attackCooldown(0.0f) {}


	int getDamage() const { 
		return damage; 
	}
	float getRange() const { 
		return range; 
	}
	float getAttackCooldown() const {
		return attackCooldown; 
	}
	

	void attack(int currX, int currY, int x, int y) {
		float distance = sqrt(pow(x - currX, 2) + pow(y - currY, 2));
		if (distance > range) {
			return;
		}

		if (currentAmmo > 0 && !fired) {
			currentAmmo--;
		}

		if (fired) {
			if (shootClock.getElapsedTime().asSeconds() >= attackCooldown) {
				fired = false;
				shootClock.restart();
			}
		}
		else {
			fired = true;
			shootClock.restart();
		}
	}

	void reload() {
		//Checks if ammo + clip is in inventory
		currentAmmo = maxAmmo;
	}
};