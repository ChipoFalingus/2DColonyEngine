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


// These don't change
enum class TraitType {
	SocialWeight,
	WorkWeight,
	BraveryWeight,
	TemperatureToleranceWeight,

	COUNT
};

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
	std::shared_ptr<Object> targetItem = nullptr;
	int targetX = 0;
	int targetY = 0;

	Evaluation(UtilityType t, float s, std::shared_ptr<Object> item = nullptr, int x = 0, int y = 0)
		: type(t), score(s), targetItem(item), targetX(x), targetY(y) {
	}
};


struct VillagerStat {
	std::string name;
	float value;
	float maxValue;
	float driftRate;

	void tick(float deltaTime) {
		value = std::min(maxValue, value + (driftRate * deltaTime));
	}

	float getNormalizedDeficiency() const {
		if (maxValue <= 0.0f) return 0.0f;
		float ratio = std::clamp(value / maxValue, 0.0f, 1.0f);

		return ratio;
	}
};

std::string activityStateToString(ActivityState state);

class Villager : public Creature {
private:
	std::vector<Job*> interrupted;
	JobType jobType;
	Job* currentJob;

public:
	std::unordered_map<SkillType, int> skills;
	std::unordered_map<TraitType, float> traits;

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
	float socialClock;

	float heatClock;

	float talkClock;

	float findChairClock;

	int moveSpeed;
	float harvestTime = 1.0f;
	float sleepTime = 5.0f;

	bool busy = false;

	bool clockRestart = false;

	Object* object_in_use;

	std::optional<std::pair<int, int>> bed = std::nullopt;

	Inventory inventory;

	bool isHungry = false;

	int thirst = 100;
	int tiredness = 0;
	int happiness = 50;
	int stress = 0;
	int social = 0;

	ActivityState activity_state;

	int preferredTemp = getRandomInt(60, 80);

	int alertness = 20;
	std::pair<int, int> lastMove = { 0,0 };

	bool retreating = false;
	Creature* threat;

	Villager* nearby;

	std::string action_log[10];

	Villager(int x, int y)
		: Creature(x, y, L'☺',
			glm::vec3(getRandomInt(100, 255),
				getRandomInt(100, 255),
				getRandomInt(100, 255)))
	{
		initTraits();

		//float agility = traits.values[static_cast<size_t>(TraitType::Agility)];
		speed = 0.1f;

		type = CreatureType::VILLAGER;
	}

	void initTraits() {

		auto skillList = getAllSkillTypes();
		int rand = getRandomInt(0, skillList.size() - 1);

		for (int i = 0; i < skillList.size(); i++) {
			if (i == rand) {
				// One random high skill
				skills[skillList[i]] = getRandomInt(10, 13);
			}
			else {
				skills[skillList[i]] = getRandomInt(1, 3);
			}
		}

		for (int i = 0; i < (int)TraitType::COUNT; i++) {
			TraitType traitType = static_cast<TraitType>(i);
			traits[traitType] = getRandomFloat(0.0f, 1.0f);
		}
	}

	Evaluation evaluateEating();
	Evaluation evaluateSleeping();
	Evaluation evaluateCombat();
	Evaluation evaluateMeditation();
	Evaluation evaluateSocializing();
	Evaluation evaluateSitting();
	Evaluation evaluateWarmingUp();

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

		if (bed) {
			return;
		}

		auto loc = findClosestItemType(xPos, yPos, 50, [&](const Object& obj, int x, int y) {
			return obj.name == "Bed" && !obj.claimed;
			});

		if (loc) {
			bed = { loc->x, loc->y };
			loc->item.lock()->claimed = true;
		}

	}

	void log(const std::string& action) {
		for (int i = 9; i > 0; i--) {
			action_log[i] = action_log[i - 1];
		}
		action_log[0] = action;
	}

	Job* getCurrentJob() {
		return currentJob;
	}

	void setJob(JobType jt) {
		jobType = jt;
	}

	void setCurrentJob(Job* job) {
		currentJob = job;
	}

	JobType getJob() {
		return jobType;
	}

	void printJobQueue() const;

	virtual ~Villager() = default;


};

