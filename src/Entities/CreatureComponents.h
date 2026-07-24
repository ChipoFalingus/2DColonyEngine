#pragma once
#include <string>
#include "Jobs/Job.h"
#include "Entities/Villager.h"

struct CreatureName {
	std::string first_name;
	std::string last_name;
};

struct Movable {
	float speed = 0.2f;
	float currentSpeed = 0.2f;

	float movementClock = 0.0f;

	int targetX = 0;
	int targetY = 0;
	bool hasTarget = true;

	std::vector<std::pair<int, int>> path;
};

struct Hostile {
	bool e = false;
};

struct TiredNeed {
	int tiredness = 0;
	float clock;
};

struct HungerNeed {
	int hunger = 100;
	float clock;
	float findFoodClock;
};

struct TemperatureNeed {
	float preferredTemp;
	float clock;
};

struct JobComponent {
	Job* currentJob;
	JobType type;
	std::vector<Job*> interrupted;
	ActivityState activity_state = ActivityState::None;

	void proposeJob(Job* newJob) {
		if (!currentJob) {
			currentJob = newJob;
			currentJob->state = JobState::Active;
			return;
		}

		if (newJob->priority > currentJob->priority) {
			interrupted.push_back(currentJob);
			currentJob->onInterrupt();
			currentJob = newJob;
			currentJob->state = JobState::Active;
		}
		else {
			delete newJob;
		}
	}
};

struct Villager {};

struct Inventory {
	entt::entity itemInHand;
	std::vector<entt::entity> inventory;
};

struct CanAttack {
	int base_damage = 1;
	int base_attack_speed = 1.0f;
};

//struct ColonyOwner {
//	Colony* colony;
//};