#pragma once
#include "Pair.h"
#include "Item.h"
#include "Tile.h"
#include "Creature.h"
#include "Inventory.h"
#include "Furniture.h"
#include "mathUtils.h"
#include "CreatureUtils.h"

#include <queue>

struct Job;

#include "JobType.h"
#include "JobCompare.h"

extern std::vector<std::string> names;

extern std::vector<std::string> lastnames;


class Villager : public Creature {
public:
	static std::vector<Villager*> allVillagers;

	JobType jobType;
	Job* currentJob;
	std::priority_queue<Job*, std::vector<Job*>, JobCompare> jobQueue;


	std::string firstname = names[getRandomInt(0, names.size() - 1)];
	std::string lastname = lastnames[getRandomInt(0, lastnames.size() - 1)];

	bool harvesting;

	sf::Clock clock;
	sf::Clock moveClock;

	int moveSpeed = 10;
	float harvestTime = 1.0f;
	float sleepTime = 5.0f;

	bool busy = false;

	bool clockRestart = false;


	std::pair<int, int> bed = {0, 0};

	Inventory inventory;

	static std::vector<std::pair<int, int>> harvestTiles;


	Villager(int x, int y)
		: Creature(x, y, L'☺',
			glm::vec3(getRandomInt(100, 255),
				getRandomInt(100, 255),
				getRandomInt(100, 255)))
	{}

	void pickUpItem(const Item& item, int amount);
	void depositItem(const Item& item, int amount);
	void doWork() override;
	//void getBestWeapon();
	std::pair<int, int> findClosestTileFurniture(const Furniture& item);

	

	virtual ~Villager() = default;


};

