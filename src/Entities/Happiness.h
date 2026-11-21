#pragma once
#include <iostream>
#include <entt/entt.hpp>

struct Thought {
	std::string id;
	std::string thought;
	int modifier;
	float duration;
};

struct Happiness {
	float current_happiness = 50;
	int base_happiness = 50;

	std::vector<Thought> active_thoughts;

	float clock = 0.0f;
};

void addThought(Thought thought, std::vector<Thought>& list);
void updateHappiness(entt::registry& registry, float dt);