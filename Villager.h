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

	// Patience: idk
	Patience,

	// Greed: idk
	Greed,
	
	COUNT
};

struct Traits {
	std::array<float, static_cast<size_t>(TraitType::COUNT)> values;
};

class Villager : public Creature {
private:
	std::priority_queue<Job*, std::vector<Job*>, JobCompare> jobQueue;
	JobType jobType;
	Job* currentJob;

public:
	Traits traits;

	std::string firstname = names[getRandomInt(0, names.size() - 1)];
	std::string lastname = lastnames[getRandomInt(0, lastnames.size() - 1)];

	bool harvesting;

	sf::Clock clock;
	sf::Clock idleClock;
	sf::Clock checkThreatsClock;
	sf::Clock moveClock;
	sf::Clock tirednessClock;
	sf::Clock eatClock;
	sf::Clock findBedClock;

	int moveSpeed;
	float harvestTime = 1.0f;
	float sleepTime = 5.0f;

	bool busy = false;

	bool clockRestart = false;


	std::pair<int, int> bed = {0, 0};

	Inventory inventory;

	static std::vector<std::pair<int, int>> harvestTiles;


	int hunger = 100;
	bool isHungry = false;

	int thirst = 100;
	int tiredness = 0;

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
		for (int i = 0; i < (int)TraitType::COUNT; i++) {
			TraitType traitType = static_cast<TraitType>(i);
			traits.values[i] = getRandomFloat(0.0f, 1.0f);
		}
	}

	void doWork() override;
	void evaluateNeeds();
	//void getBestWeapon();

	void pickUpItem(std::shared_ptr<Object> item, int x, int y, Stockpile* stockpile = nullptr);
	void dropItem(std::shared_ptr<Object> item, int x, int y);

	void retreat(Creature* threat);

	void claimBed() {

		auto loc = findClosestItemType(xPos, yPos, 50, [&](const Object& obj) {
			return obj.name == "Bed" && !obj.claimed;
			});

		if (loc) {
			bed = { loc->x, loc->y };
			loc->item->claimed = true;
		}

	}

	void addToJobQueue(Job* job) {
		jobQueue.push(job);
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

