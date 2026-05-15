#pragma once
#include <vector>
#include <functional>
#include "Item.h"
#include "Crop.h"
#include "Tool.h"
#include "Creature.h"
#include "JobType.h"
#include "Crafting.h"
#include "ItemLocation.h"
#include "Food.h"


class Villager;

enum class JobState {
	Active,
	Queued,
	Waiting,
	Completed,
	Failed
};


struct Job {
	Villager* villager = nullptr; // Who is assigned to the job
	Tool* preferredTool = nullptr;
	std::string preferredToolName;
	JobType type;
	JobState state = JobState::Queued;

	int x, y; // Where the job requires you to be
	int priority = 0; // Higher priority jobs get assigned first

	Job(Villager* v, Tool* preferredTool, JobType jobtype)
		:villager(v), preferredTool(preferredTool), type(jobtype) {

		if (preferredTool) {
			preferredToolName = preferredTool->name;
		}
	}

	virtual ~Job() = default;

	virtual void update() {}
	virtual void onComplete() {}
	virtual void onFail() {}
};



class JobManager {
public:
	// Holds all available jobs and gives them to villagers whenever possible
	static std::vector<Job*> JobList;
	static void findBestColonistForJob(Job& job);
	static void findJobForColonist(Villager& v);
	static void addJob(Job* job) {
		JobList.push_back(job);
	}
	static void removeJob(Job* job) {
		JobList.erase(
			std::remove(JobList.begin(), JobList.end(), job),
			JobList.end()
		);
	}
};

class Harvest : public Job {
public:
	bool itemFound = false;
	std::shared_ptr<Object> item;


	Harvest(Villager* v, Tool* tool, JobType job, std::shared_ptr<Object> i)
		: Job(v, tool, job), item(i)
	{}

	void update();
};

class HarvestTile : public Job {
public:
	std::string item;
	int locX, locY;
	bool isHarvesting = false;

	HarvestTile(Villager* v, Tool* tool, JobType job, std::string i, int locX, int locY)
		: Job(v, tool, job), item(i), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
	}

	void update();
};


class Plant : public Job {
public:
	int locX, locY;
	std::string name;

	Plant(Villager* v, Tool* tool, JobType job, std::string name, int locX, int locY)
		: Job(v, tool, job), name(name), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
	}

	void update();

};

class Build : public Job {
public:
	std::string itemName;
	int locX, locY;
	bool grabbedAllItems = false;
	bool init = false;
	std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>> reserve;

	Recipe* recipe;
	std::unordered_map<std::string, int> ingredients;
	Build(Villager* v, Tool* tool, JobType job, std::string item, int locX, int locY)
		: Job(v, tool, job), itemName(item), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
		recipe = RecipeRegistry::getInstance().get(item);
		ingredients = recipe->ingredients;
	}

	void update();
};

class PlaceItem : public Job {
public:
	Object* itemToPlace;
	int locX, locY;

	PlaceItem(Villager* v, Tool* tool, JobType job, Object* item, int locX, int locY)
		: Job(v, tool, job), itemToPlace(item), locX(locX), locY(locY)
	{}

	void update();
};

class Idle : public Job {
public:

	float waitTime = 0.0f;
	bool initialized = false;

	Idle(Villager* v, Tool* tool, JobType job)
		: Job(v, tool, job)
	{}

	void update();
};

class Attack : public Job {
public:

	Creature* target;

	Attack(Villager* v, Tool* tool, JobType job, Creature* target)
		: Job(v, tool, job), target(target)
	{}

	void update();
};

class Retreat : public Job {
public:

	Creature* threat;

	Retreat(Villager* v, Tool* tool, JobType job, Creature* threat)
		: Job(v, tool, job), threat(threat)
	{}

	void update();
};

class Sleep : public Job {
public:
	bool sleeping = false;

	Sleep(Villager* v, Tool* tool, JobType job)
		: Job(v, tool, job)
	{}

	void update();
};

class Craft : public Job {
public:
	std::string itemName;
	std::unordered_map<std::string, int> ingredients;
	std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>> reserve;
	bool init = false;
	bool grabbedAllItems = false;

	Recipe* recipe;
	Craft(Villager* v, Tool* tool, JobType job, std::string item)
		: Job(v, tool, job), itemName(item)
	{
		recipe = RecipeRegistry::getInstance().get(item);
		ingredients = recipe->ingredients;
	}
	void update();
};


class Move : public Job {
public:
	int toX;
	int toY;
	Move(Villager* v, Tool* tool, JobType job, int x, int y)
		: Job(v, tool, job), toX(x), toY(y)
	{
	}
	void update();
};

class MoveItem : public Job {
public:
	std::shared_ptr<Object> itemToMove;
	int fromX, fromY;
	int toX, toY;

	bool itemPickedUp = false;
	bool claimedSpot = false;

	MoveItem(Villager* v, Tool* tool, JobType job, std::shared_ptr<Object> item, int fX, int fY, int tX, int tY)
		: Job(v, tool, job), itemToMove(item), fromX(fX), fromY(fY), toX(tX), toY(tY)
	{
		x = fX;
		y = fY;
	}
	void update();
};

class FindFood : public Job {
public:

	enum State {
		Find,
		Grab,
		Eat,
	};

	float eatTimer = 0.0f;
	int tX, tY;
	std::shared_ptr<Food> food;
	std::optional<ItemLocation> place = std::nullopt;

	State foodState = State::Grab;

	FindFood(Villager* v, Tool* tool, JobType job, int x, int y, std::shared_ptr<Food> food)
		: Job(v, tool, job), tX(x), tY(y), food(food)
	{}
	void update();
};

class Sit : public Job {
public:
	int tX, tY;
	bool init = false;
	Sit(Villager* v, Tool* tool, JobType job, int x, int y)
		: Job(v, tool, job), tX(x), tY(y)
	{
	}
	~Sit();

	void update();
	
};

class Wander : public Job {

public:
	int tX;
	int tY;
	bool hasTarget = false;
	Wander(Villager* v, Tool* tool, JobType job)
		: Job(v, tool, job)
	{}

	void update();
	void pickNewTarget();
};