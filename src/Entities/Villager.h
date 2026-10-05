#pragma once
#include <queue>
#include <array>

#include <entt/entt.hpp>

entt::entity spawnVillager(int x, int y);
void VillagerSystem(float deltaTime);

void updateHunger();
void updateTiredness();
void updateWork();
void updateAttack();


enum class ActivityState {
	None,
	Sitting,
	Sleeping,
	Eating,
	Wandering,
	Working,
	Socializing,
	Attacking,
	Retreating,
};

std::string activityStateToString(ActivityState state);