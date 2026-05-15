#pragma once

#include "Object.h"

class Structure : public Object {
public:
	int health;
	int maxHealth = 100;

	Structure() : Object(), health(maxHealth) {}
};