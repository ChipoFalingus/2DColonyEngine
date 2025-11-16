#pragma once
#include "Item.h"

class Gun : public Item {
public:

	int maxAmmo;
	int currentAmmo;
	int damage;
	float range;
	bool fired = false;

	float attackCooldown; //in seconds
	sf::Clock shootClock;

	Gun(std::string name, wchar_t ch, sf::Color color, int maxAmmo, int damage, float range, float attackCooldown) :
		maxAmmo(maxAmmo), damage(damage), range(range), attackCooldown(attackCooldown) {

		this->name = name;
		displayChar = ch;
		displayColor = color;

	}


	int getDamage() const override { 
		return damage; 
	}
	float getRange() const override { 
		return range; 
	}
	float getAttackCooldown() const override {
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


extern Gun PISTOL;
extern Gun PEPPER_GUN;
extern Gun ASSAULT_RIFLE;
extern Gun MINIGUN;
extern Gun MEGA_SNIPER;