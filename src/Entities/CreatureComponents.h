#pragma once
#include <string>
#include "Jobs/Job.h"
#include "Jobs/JobUtils.h"
#include "Entities/Villager.h"
#include "Utility/mathUtils.h"

struct CreatureName {
	std::string first_name;
	std::string last_name;
};

struct Hostile {
	bool e = false;
};

struct TiredNeed {
	int tiredness = 0;
	float clock;
};

struct Ownership {
	entt::entity ownedBed = entt::null;
	uint32_t ownedRoomID = 0;
};

struct HungerNeed {
	int hunger = 0;
	float weight = getRandomFloat(0.0f, 1.0f);
	float clock;
	float findFoodClock;
};

struct TemperatureNeed {
	float preferredTemp;
	float clock = 0.0f;
	
	// 0 is min, 1 is max
	float temp_comfort = 0.0f;
};

struct JobComponent {
	Job* currentJob;
	JobType type;
	std::vector<Job*> interrupted;
	ActivityState activity_state = ActivityState::None;

	float panicClock = 0.0f;
	float panicDuration = 0.0f;

	bool willAccept(Job* newJob) {
		return !currentJob || newJob->priority > currentJob->priority;
	}

	void proposeJob(Job* newJob) {
		if (!currentJob) {
			currentJob = std::move(newJob);
			currentJob->state = JobState::Active;
			return;
		}

		if (newJob->priority > currentJob->priority) {
			interrupted.push_back(currentJob);
			currentJob->onInterrupt();
			currentJob = std::move(newJob);
			currentJob->state = JobState::Active;
		}
	}

	void clearInterruptedJobs() {
		for (Job* job : interrupted) {
			job->villager = entt::null;
		}
		interrupted.clear();
	}
};

struct Villager {};
struct Zombie {};

struct CombatComponent {
	int base_damage = 1;
	int base_attack_speed = 1.0f;

	ItemLocation target;
	float checkThreatsClock = 0.0f;

	float bravery = getRandomFloat(0.0f, 1.0f);
};

struct Social {
	int social = 100;

	float searchClock = 0.0f;
	float clock = 0.0f;
};

struct LightNeed {
	float minLight = 0.6f;
};

struct Equipment {
	entt::entity item;
};

struct OnGuard {
	entt::entity guardPost = entt::null;

	float clock = 0.0f;
	float duration = 0.0f;
};;

//struct ColonyOwner {
//	Colony* colony;
//};

struct Icon {
	wchar_t icon = L'!';
};