#pragma once
#include <vector>
#include <functional>
#include "Item.h"
#include "Crop.h"
#include "Tool.h"
#include "Creature.h"
#include "JobType.h"


class Villager;

struct Job {
	Villager* villager;
	Tool* preferredTool = nullptr;
	JobType preferredJob;
	int x, y; // Where the job requires you to be
	int priority;
	bool completed = false;

	Job(Villager* v, JobType jobtype)
		:villager(v), preferredJob(jobtype), x(x), y(y) {
	}

	virtual void update() {}
};



class JobManager {
public:
	// Holds all available jobs and gives them to villagers whenever possible
	static std::vector<Job*> JobList;

	static void findBestColonistForJob(Job& job);
	static void addJob(Job* job) {
		JobList.push_back(job);
	}
	static void removeJob();
};

class Harvest : public Job {
public:
	bool itemFound = false;
	Item& item;


	Harvest(Villager* v, JobType job, Item* i)
		: Job(v, job), item(*i)
	{}

	void update();
};

class HarvestTile : public Job {
public:
	Item& item;
	int locX, locY;
	bool isHarvesting = false;

	HarvestTile(Villager* v, JobType job, Item* i, int x, int y)
		: Job(v, job), item(*i), locX(x), locY(y)
	{}

	void update();
};


class Plant : public Job {
public:
	int locX, locY;
	Crop& crop;

	Plant(Villager* v, JobType job, Crop* crop, int locX, int locY)
		: Job(v, job), crop(*crop), locX(locX), locY(locY)
	{}

	void update();

};

class Build : public Job {
public:
	Item& itemToBuild;
	int locX, locY;

	Build(Villager* v, JobType job, Item* item, int locX, int locY)
		: Job(v, job), itemToBuild(*item), locX(locX), locY(locY)
	{}

	void update();
};

class PlaceItem : public Job {
public:
	Item& itemToPlace;
	int locX, locY;

	PlaceItem(Villager* v, JobType job, Item* item, int locX, int locY)
		: Job(v, job), itemToPlace(*item), locX(locX), locY(locY)
	{}

	void update();
};

class Idle : public Job {
public:

	Idle(Villager* v, JobType job)
		: Job(v, job)
	{}

	void update();
};

class Attack : public Job {
public:

	Creature* target;
	std::function<bool(Creature*)> targetFilter;

	Attack(Villager* v, JobType job, std::function<bool(Creature*)> targetFilter)
		: Job(v, job), targetFilter(targetFilter)
	{}

	void update();
};

class Retreat : public Job {
public:

	Creature* threat;

	Retreat(Villager* v, JobType job, Creature* threat)
		: Job(v, job), threat(threat)
	{}

	void update();
};

class Sleep : public Job {
public:
	bool sleeping = false;

	Sleep(Villager* v, JobType job)
		: Job(v, job)
	{}

	void update();
};

class Move : public Job {
public:
	Move(Villager* v, JobType job)
		: Job(v, job)
	{
	}
	void update();
};

class MoveItem : public Job {
	public:
	Item itemToMove;
	int fromX, fromY;
	int toX, toY;
	bool itemPickedUp = false;
	MoveItem(Villager* v, JobType job, Item* item, int fX, int fY, int tX, int tY)
		: Job(v, job), itemToMove(*item), fromX(fX), fromY(fY), toX(tX), toY(tY)
	{}
	void update();
};