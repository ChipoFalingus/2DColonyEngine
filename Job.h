#pragma once
#include <vector>
#include <functional>
#include "Item.h"
#include "Creature.h"
#include "JobType.h"
#include "Crafting.h"
#include "ItemLocation.h"
#include "HarvestRules.h"


//class Villager;
//class HeatEmitter;
//
enum class JobState {
	Active,
	Queued,
	Waiting,
	Completed,
	Failed
};


struct Job {
	entt::entity villager = entt::null; // Who is assigned to the job
	entt::entity preferredTool = entt::null;
	std::string preferredToolName;
	SkillType type;
	JobState state = JobState::Queued;

	int x, y; // Where the job requires you to be
	int priority = 0; // Higher priority jobs get assigned first

	Job(entt::entity v, entt::entity preferredTool, SkillType skillType)
		:villager(v), preferredTool(preferredTool), type(skillType) {

		if (preferredTool != entt::null) {
		}
	}

	virtual ~Job() = default;

	virtual void update() {}
	virtual void onInterrupt() {}
	//virtual void waitingUpdate() {}
	//virtual void onFail() {}
};



class JobManager {
public:
	// Holds all available jobs and gives them to villagers whenever possible
	static std::vector<Job*> JobList;

	//static void findBestColonistForJob(Job& job);
	//static void findJobForColonist(Villager& v);
	static void addJob(Job* job) {
		JobList.push_back(job);
	}
	static void removeJob(Job* job) {
		JobList.erase(
			std::remove(JobList.begin(), JobList.end(), job),
			JobList.end()
		);
		delete job;
	}


	static void assignJobs();

	static void update();
};

class Harvest : public Job {
public:
	bool itemFound = false;
	entt::entity item;


	Harvest(entt::entity v, entt::entity tool, SkillType skillType, entt::entity i)
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

	entt::entity item;
	int locX, locY;

	float harvestClock = 0.0f;

	State harvestState = State::MovingToTile;

	HarvestTile(entt::entity v, entt::entity tool, SkillType skill, entt::entity i, int locX, int locY)
		: Job(v, tool, skill), item(i), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
	}

	void update();
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

	Plant(entt::entity v, entt::entity tool, SkillType skillType, std::string seed, int locX, int locY)
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
	entt::entity staticRecipeEntity = entt::null;

	int locX, locY;
	bool grabbedAllItems = false;
	bool init = false;
	std::vector<std::pair<std::pair<int, int>, entt::entity>> reserve;

	Build(entt::entity v, entt::entity tool, SkillType skillType, std::string itemName, int locX, int locY)
		: Job(v, tool, skillType), itemName(itemName), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;

	}

	void update();
};

class BuildFurniture : public Job {
public:

	enum State {
		Getting,
		Placing
	};

	entt::entity item;
	int fX, fY;
	int tX, tY;
	bool grabbedItem = false;
	bool init = false;

	State jobState = State::Getting;

	BuildFurniture(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int fX, int fY, int tX, int tY)
		: Job(v, tool, skillType), item(item), fX(fX), fY(fY), tX(tX), tY(tY)
	{
		x = fX;
		y = fY;
	}

	void update();
};

class Refuel : public Job {
public:
	entt::entity fuel;
	entt::entity target;
	int fX, fY;

	enum State {
		Getting,
		Fueling
	};

	State jobState = State::Getting;

	Refuel(entt::entity v, entt::entity tool, SkillType skillType, entt::entity fuel, entt::entity target, int fX, int fY)
		: Job(v, tool, skillType), fuel(fuel), target(target), fX(fX), fY(fY)
	{
	}

	void update();
};

class PlaceItem : public Job {
public:
	entt::entity itemToPlace;
	int locX, locY;

	PlaceItem(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int locX, int locY)
		: Job(v, tool, skillType), itemToPlace(item), locX(locX), locY(locY)
	{}

	void update();
};

class Idle : public Job {
public:

	float waitTime = 0.0f;
	float clock = 0.0f;
	bool initialized = false;

	Idle(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{}

	void update();
	void onInterrupt();
};

class Attack : public Job {
public:

	entt::entity target;
	float attackClock;

	Attack(entt::entity v, entt::entity tool, SkillType skillType, entt::entity target)
		: Job(v, tool, skillType), target(target)
	{}

	void update();
};

class Retreat : public Job {
public:

	entt::entity threat;

	Retreat(entt::entity v, entt::entity tool, SkillType skillType, entt::entity threat)
		: Job(v, tool, skillType), threat(threat)
	{}

	void update();
};

class Sleep : public Job {
public:
	bool sleeping = false;
	bool lookedForBed = false;

	Sleep(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{}

	void update();
};

class Craft : public Job {
public:
	enum State {
		FetchingItems,
		MovingToBench,
		Crafting
	};

	State jobState = State::FetchingItems;

	std::string item;
	entt::entity staticRecipeEntity = entt::null;

	std::unordered_map<std::string, int> ingredients;
	std::vector<std::pair<std::pair<int, int>, entt::entity>> reserve;
	bool init = false;
	bool grabbedAllItems = false;

	Craft(entt::entity v, entt::entity tool, SkillType skillType, std::string item)
		: Job(v, tool, skillType), item(item)
	{
	}

	void update();
};


class Move : public Job {
public:
	int toX;
	int toY;
	Move(entt::entity v, entt::entity tool, SkillType skillType, int x, int y)
		: Job(v, tool, skillType), toX(x), toY(y)
	{
	}
	void update();
};

class MoveItem : public Job {
public:
	entt::entity itemToMove;
	int fromX, fromY;
	int toX, toY;

	bool itemPickedUp = false;
	bool claimedSpot = false;

	MoveItem(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int fX, int fY, int tX, int tY)
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

	entt::entity itemToMove;
	int fromX, fromY;
	int toX, toY;

	State moveState = State::PickUpItem;

	HaulToStockpile(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int fX, int fY, int tX, int tY)
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
	entt::entity food;
	std::optional<ItemLocation> place = std::nullopt;

	State foodState = State::Grab;

	FindFood(entt::entity v, entt::entity tool, SkillType skillType, int x, int y, entt::entity food)
		: Job(v, tool, skillType), tX(x), tY(y), food(food)
	{}
	void update();
};

class Sit : public Job {
public:
	int tX, tY;
	entt::entity chair;
	bool init = false;

	float clock;

	Sit(entt::entity v, entt::entity tool, SkillType skillType, entt::entity chair, int x, int y)
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
	Wander(entt::entity v, entt::entity tool, SkillType skillType)
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
	Meditate(entt::entity v, entt::entity tool, SkillType skillType)
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

	entt::entity other;
	float clock;

	State talkState = State::WalkTo;

	Talk(entt::entity v, entt::entity tool, SkillType skillType, entt::entity other)
		: Job(v, tool, skillType), other(other)
	{}
	void update();
};