#pragma once
#include <queue>
#include <array>

#include <entt/entt.hpp>

entt::entity spawnVillager(int x, int y);
void VillagerSystem(float deltaTime);

void updateHunger();
void updateTiredness();
void updateWork();


enum class ActivityState {
	None,
	Sitting,
	Sleeping,
	Eating,
	Wandering,
	Working,
	Meditating,
	Socializing,
	Attacking,
	Retreating,
};

std::string activityStateToString(ActivityState state);

enum class UtilityType { 
	IDLE, 
	ATTACK, 
	RETREAT, 
	SLEEP, 
	EAT, 
	CURRENT,
	MEDITATE,
	SIT,
	WANDER,
	SOCIALIZE,
	WARMING_UP,
	INTERRUPTED_RESUME, 
};

struct Evaluation {
	UtilityType type;
	float score;
	entt::entity targetItem = entt::null;
	int targetX = 0;
	int targetY = 0;

	Evaluation(UtilityType t, float s, entt::entity item = entt::null, int x = 0, int y = 0)
		: type(t), score(s), targetItem(item), targetX(x), targetY(y) {
	}
};