#pragma once
#include "Pair.h"
#include "Item.h"
#include "Tile.h"
#include "Creature.h"
#include "Inventory.h"
#include "mathUtils.h"
#include "CreatureUtils.h"
#include "Stockpile.h"
#include "ItemUtils.h"
#include "Clock.h"

#include <queue>
#include <array>

struct Job;

#include "JobType.h"
#include "JobCompare.h"

extern std::vector<std::string> names;

extern std::vector<std::string> lastnames;

enum class TraitType {
	// Strength: Affects carrying capacity and melee combat effectiveness
	Strength,

	// Agility: Influences movement speed
	Agility,

	// Intelligence: Improves crafting ability
	Intelligence,

	// Charisma: Affects dealing with other villagers
	Charisma,

	// Resilience: Determines resistance to injuries
	Resilience,

	// Metabolism: Affects hunger and thirst rates
	Metabolism,

	// Bravery: Will affect retreating and engaging in combat
	Bravery,

	// Curiosity: Affects willingness to explore
	Curiosity,

	// Sociability: Affects trading
	Sociability,
	
	COUNT
};

struct Traits {
	std::array<float, static_cast<size_t>(TraitType::COUNT)> values;
};

enum class ActivityState {
	None,
	Sitting,
	Sleeping,
	Eating,
	Wandering,
	Working
};


std::string activityStateToString(ActivityState state);

class Villager : public Creature {
private:
	// replace with vector
	//std::priority_queue<Job*, std::vector<Job*>, JobCompare> jobQueue;
	std::vector<Job*> jobQueue;
	JobType jobType;
	Job* currentJob;


public:
	Traits traits;

	std::unordered_map<SkillType, int> skills;

	std::string firstname = names[getRandomInt(0, names.size() - 1)];
	std::string lastname = lastnames[getRandomInt(0, lastnames.size() - 1)];

	float clock;
	float idleClock;
	float checkThreatsClock;
	float moveClock;
	float tirednessClock;
	float hungerClock;
	float findBedClock;
	float findFoodClock;

	int moveSpeed;
	float harvestTime = 1.0f;
	float sleepTime = 5.0f;

	bool busy = false;

	bool clockRestart = false;

	Object* object_in_use;


	std::pair<int, int> bed = {0, 0};

	Inventory inventory;

	bool isHungry = false;

	int thirst = 100;
	int tiredness = 0;

	ActivityState activity_state;


	int alertness = 20;
	std::pair<int, int> lastMove = { 0,0 };

	bool retreating = false;
	Creature* threat;

	Villager(int x, int y)
		: Creature(x, y, L'☺',
			glm::vec3(getRandomInt(100, 255),
				getRandomInt(100, 255),
				getRandomInt(100, 255)))
	{
		initTraits();

		//float agility = traits.values[static_cast<size_t>(TraitType::Agility)];
		speed = 0.1f;
	}

	void initTraits() {

		int skillListSize = getAllSkillTypes().size();
		int rand = getRandomInt(0, skillListSize - 1);

		for (int i = 0; i < skillListSize; i++) {
			SkillType skillType = static_cast<SkillType>(i);
			if (i == rand) {
				// One random high skill
				skills[skillType] = getRandomInt(10, 13);
			}
			else {
				skills[skillType] = getRandomInt(1, 3);
			}
		}

		for (int i = 0; i < (int)TraitType::COUNT; i++) {
			TraitType traitType = static_cast<TraitType>(i);
			traits.values[i] = getRandomFloat(0.0f, 1.0f);
		}
	}

	void sense();
	void idle();
	void decide();
	void move();

	void doWork() override;
	//void getBestWeapon();

	void pickUpItem(std::shared_ptr<Object> item, int x, int y, Stockpile* stockpile = nullptr);
	void dropItem(std::shared_ptr<Object> item, int x, int y);

	void retreat(Creature* threat);

	void claimBed() {

		auto loc = findClosestItemType(xPos, yPos, 50, [&](const Object& obj, int x, int y) {
			return obj.name == "Bed" && !obj.claimed;
			});

		if (loc) {
			bed = { loc->x, loc->y };
			loc->item.lock()->claimed = true;
		}

	}

	void addToJobQueue(Job* job) {
		jobQueue.push_back(job);
	}

	void setJob(JobType jt) {
		jobType = jt;
	}

	int getJobQueueSize() {
		return jobQueue.size();
	}

	JobType getJob() {
		return jobType;
	}

	virtual ~Villager() = default;


};

