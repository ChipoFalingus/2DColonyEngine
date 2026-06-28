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
#include "HarvestRules.h"


class Villager;
class HeatEmitter;

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
	SkillType type;
	JobState state = JobState::Queued;

	int x, y; // Where the job requires you to be
	int priority = 0; // Higher priority jobs get assigned first

	Job(Villager* v, Tool* preferredTool, SkillType skillType)
		:villager(v), preferredTool(preferredTool), type(skillType) {

		if (preferredTool) {
			preferredToolName = preferredTool->name;
		}
	}

	virtual ~Job() = default;

	virtual void update() {}
	virtual void waitingUpdate() {}
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


	static void assignJobs();

	static void update();
};

class Harvest : public Job {
public:
	bool itemFound = false;
	std::shared_ptr<Object> item;


	Harvest(Villager* v, Tool* tool, SkillType skillType, std::shared_ptr<Object> i)
		: Job(v, tool, skillType), item(i)
	{}

	void update();
};

class HarvestTile : public Job {
public:

	enum State {
		GrabbingTool,
		MovingToTile,
		Harvesting,
	};

	std::string item;
	int locX, locY;
	bool isHarvesting = false;
	bool addedGetToolJob = false;
	bool gotTool = false;

	Rule* rule;
	float searchTimer = 0.0f;

	State harvestState = State::MovingToTile;

	HarvestTile(Villager* v, Tool* tool, SkillType skillType, std::string i, int locX, int locY)
		: Job(v, tool, skillType), item(i), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;

		rule = HarvestRuleRegistry::getInstance().get(item);
	}

	void update();
	void waitingUpdate();
};


class Plant : public Job {
public:

	enum State {
		GettingSeed,
		Tilling,
		Planting
	};

	int locX, locY;
	std::string seed;

	float plantClock;
	float tillClock;

	State plantState = State::GettingSeed;

	Plant(Villager* v, Tool* tool, SkillType skillType, std::string seed, int locX, int locY)
		: Job(v, tool, skillType), seed(seed), locX(locX), locY(locY)
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
	Build(Villager* v, Tool* tool, SkillType skillType, std::string item, int locX, int locY)
		: Job(v, tool, skillType), itemName(item), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
		recipe = RecipeRegistry::getInstance().get(item);
		ingredients = recipe->ingredients;
	}

	void update();
};

class BuildFurniture : public Job {
public:

	enum State {
		Getting,
		Placing
	};

	std::weak_ptr<Object> itemName;
	int fX, fY;
	int tX, tY;
	bool grabbedItem = false;
	bool init = false;

	State jobState = State::Getting;

	BuildFurniture(Villager* v, Tool* tool, SkillType skillType, std::weak_ptr<Object> item, int fX, int fY, int tX, int tY)
		: Job(v, tool, skillType), itemName(item), fX(fX), fY(fY), tX(tX), tY(tY)
	{
		x = fX;
		y = fY;
	}

	void update();
};

class Refuel : public Job {
public:
	std::weak_ptr<Object> fuel;
	std::weak_ptr<HeatEmitter> target;
	int fX, fY;

	enum State {
		Getting,
		Fueling
	};

	State jobState = State::Getting;

	Refuel(Villager* v, Tool* tool, SkillType skillType, std::weak_ptr<Object> fuel, std::weak_ptr<HeatEmitter> target, int fX, int fY)
		: Job(v, tool, skillType), fuel(fuel), target(target), fX(fX), fY(fY)
	{
	}

	void update();
};

class PlaceItem : public Job {
public:
	Object* itemToPlace;
	int locX, locY;

	PlaceItem(Villager* v, Tool* tool, SkillType skillType, Object* item, int locX, int locY)
		: Job(v, tool, skillType), itemToPlace(item), locX(locX), locY(locY)
	{}

	void update();
};

class Idle : public Job {
public:

	float waitTime = 0.0f;
	bool initialized = false;

	Idle(Villager* v, Tool* tool, SkillType skillType)
		: Job(v, tool, skillType)
	{}

	void update();
};

class Attack : public Job {
public:

	Creature* target;
	float attackClock;

	Attack(Villager* v, Tool* tool, SkillType skillType, Creature* target)
		: Job(v, tool, skillType), target(target)
	{}

	void update();
};

class Retreat : public Job {
public:

	Creature* threat;

	Retreat(Villager* v, Tool* tool, SkillType skillType, Creature* threat)
		: Job(v, tool, skillType), threat(threat)
	{}

	void update();
};

class Sleep : public Job {
public:
	bool sleeping = false;
	bool lookedForBed = false;

	Sleep(Villager* v, Tool* tool, SkillType skillType)
		: Job(v, tool, skillType)
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
	Craft(Villager* v, Tool* tool, SkillType skillType, std::string item)
		: Job(v, tool, skillType), itemName(item)
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
	Move(Villager* v, Tool* tool, SkillType skillType, int x, int y)
		: Job(v, tool, skillType), toX(x), toY(y)
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

	MoveItem(Villager* v, Tool* tool, SkillType skillType, std::shared_ptr<Object> item, int fX, int fY, int tX, int tY)
		: Job(v, tool, skillType), itemToMove(item), fromX(fX), fromY(fY), toX(tX), toY(tY)
	{
		x = fX;
		y = fY;
	}
	void update();
};

class HaulToStockpile : public Job {
public:
	enum State {
		PickUpItem,
		Move,
		Drop,
	};

	std::shared_ptr<Object> itemToMove;
	int fromX, fromY;
	int toX, toY;

	State moveState = State::PickUpItem;

	HaulToStockpile(Villager* v, Tool* tool, SkillType skillType, std::shared_ptr<Object> item, int fX, int fY, int tX, int tY)
		: Job(v, tool, skillType), itemToMove(item), fromX(fX), fromY(fY), toX(tX), toY(tY)
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

	FindFood(Villager* v, Tool* tool, SkillType skillType, int x, int y, std::shared_ptr<Food> food)
		: Job(v, tool, skillType), tX(x), tY(y), food(food)
	{}
	void update();
};

class Sit : public Job {
public:
	int tX, tY;
	std::weak_ptr<Object> chair;
	bool init = false;

	float clock;

	Sit(Villager* v, Tool* tool, SkillType skillType, std::weak_ptr<Object> chair, int x, int y)
		: Job(v, tool, skillType), chair(chair), tX(x), tY(y)
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
	Wander(Villager* v, Tool* tool, SkillType skillType)
		: Job(v, tool, skillType)
	{}

	void update();
	void pickNewTarget();
};


class Meditate : public Job {
public:
	int tX;
	int tY;
	bool hasTarget = false;
	Meditate(Villager* v, Tool* tool, SkillType skillType)
		: Job(v, tool, skillType)
	{}
	void update();
	void pickNewTarget();
};

class Talk : public Job {
public:
	enum State {
		WalkTo,
		TalkTo
	};

	Villager* other;
	float clock;

	State talkState = State::WalkTo;

	Talk(Villager* v, Tool* tool, SkillType skillType, Villager* other)
		: Job(v, tool, skillType), other(other)
	{}
	void update();
};