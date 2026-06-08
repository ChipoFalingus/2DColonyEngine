#pragma once
#include "Creature.h"

enum class monsterState { IDLE, WANDERING, ATTACKING };

class Monster : public Creature {
public:
	sf::Color displayColor;
	sf::String displayChar;

	bool hostile = true;

	monsterState state = monsterState::IDLE;

	void doWork() override;
	
	Monster(int x, int y, wchar_t ch, glm::vec3 color)
		: Creature(x, y, ch, color)
	{}

};