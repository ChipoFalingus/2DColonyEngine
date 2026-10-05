#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Jobs/JobType.h"

// how many structs could a guy need?

// probably need to split this up into different files because 1 change makes recompilation takes an agonizing amount of time

struct Renderable {
	wchar_t character;
	glm::vec3 color;
};

struct Name {
	std::string name;
};

struct Claimable {
	bool claimed;
};

struct Position {
	int x, y;
};

struct Drop {
	std::string item;
	float odds;
};

struct Harvestable {
	std::vector<Drop> drops;
	SkillType requiredSkill;
};

struct Health {
	int health;
};

struct Nutritional {
	int nutrition;
};

struct ProduceSpawner {
	std::string produce;
	float produceClock;
	float productionTime = 10.0f;

	int range = 1;

	bool isProducing = false;
	bool canProduce = true;
};

enum class CropState {
	SPROUT,
	MATURED,
	DEAD
};

struct CropStage {
	wchar_t character;
	glm::vec3 color;
};

// crop stuff
struct Sprouting {};
struct Matured {};
struct Dead {};

struct Crop {
	std::vector<CropStage> growthStages;

	float growthTime;
	float currentGrowthTime;
	float growthClock;

	int growthStage;
	int growthStageMax;

	int mature_stage;
	int death_age;

	CropState state = CropState::SPROUT;
};

struct HeatEmitter {
	float heat_intensity;
	bool addedToHeatMap = false;
};

struct LightEmitter {
	float light_intensity;
	bool addedToLightMap = false;
};

struct FuelBurner {
	float fuel_amount;
	float burn_rate;
};

// Requires FuelBurner component
struct Furnace {
	entt::entity itemToCook;
};

struct Gun {
	float range;
	int damage;
	float fire_rate;
	int ammo_capacity;

	float reload_time;
	float fire_clock = 0.0f;
	float reload_clock = 0.0f;
	int current_ammo = 40000;

	bool empty = false;
	bool reloading = false;

	bool isEmpty() const { return current_ammo <= 0; }
	bool canFire() const { return current_ammo > 0 && !reloading; }

	void shoot() { current_ammo--; }
};

struct Craftable {
	std::unordered_map<std::string, int> ingredients;
	std::string benchRequired;

	float craftTime;
	int quantity;
};

struct Seed {
	std::string produce;
};

struct Furniture {
	bool active;
};

struct Spawner {
	std::string spawn;
	float cooldown;
	int cap;

	float clock = 0.0f;
};

struct Sittable {
	bool i;
};
struct Table {};
struct Structure {
	bool flammable;
};

// makes tiles unwalkable
struct BlocksTile {};

struct Bed {
	bool i;
};

struct BlueprintTag {};

// add colony variable
struct NeedsMoving {
	bool foundSpot = false;
};