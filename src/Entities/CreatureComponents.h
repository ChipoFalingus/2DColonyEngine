#pragma once
#include <string>
#include "Jobs/Job.h"
#include "Entities/Villager.h"
#include "Utility/mathUtils.h"

struct CreatureName {
	std::string first_name;
	std::string last_name;
};

enum class MovementMode : uint8_t {
	Idle,
	Directional,
	Path
};

struct Movable {
	float speed = 0.8f;
	float currentSpeed = 0.2f;

	float movementClock = 0.0f;

	int targetX = 0;
	int targetY = 0;
	bool hasTarget = false;

	std::vector<std::pair<int, int>> path;

	MovementMode movementMode = MovementMode::Idle;

	// Squad movement
	int dirX = 0;
	int dirY = 0;

	bool ignoreSeparation = false;

	void newPath(int newX, int newY) {
		path.clear();
		targetX = newX;
		targetY = newY;
	}
};

struct Hostile {
	bool e = false;
};

struct TiredNeed {
	int tiredness = 0;
	float clock;

	std::optional<std::pair<int, int>> bedLocation;
};

struct HungerNeed {
	int hunger = 100;
	float weight = getRandomFloat(0.0f, 1.0f);
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

	float panicClock = 0.0f;
	float panicDuration = 0.0f;

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

	void clearInterruptedJobs() {
		for (Job* job : interrupted) {
			job->villager = entt::null;
			evaluateJobDanger(job);
		}
		interrupted.clear();
	}
};

struct Villager {};
struct Zombie {};

struct Inventory {
	entt::entity itemInHand;
	std::vector<entt::entity> inventory;
};

struct CombatComponent {
	entt::entity equippedWeapon;
	int base_damage = 1;
	int base_attack_speed = 1.0f;

	ItemLocation target;
	float checkThreatsClock = 0.0f;

	float bravery = getRandomFloat(0.0f, 1.0f);
};

struct Social {
	int social = getRandomInt(85, 100);

	float searchClock = 0.0f;
	float clock = 0.0f;
};

struct LightNeed {
	float minLight = 0.6f;
};

//struct ColonyOwner {
//	Colony* colony;
//};