#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Jobs/JobType.h"

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

struct Harvestable {
	std::string produce;
	//int quantity;
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
	float productionTime = 5.0f;

	bool isProducing = false;
};

struct Crop {
	std::vector<std::pair<wchar_t, glm::vec3>> growthStages;
	std::string produce;

	float growthTime;
	float growthClock;

	int growthStage;
	int growthStageMax;
};

struct HeatEmitter {
	float heat_intensity;
};

struct LightEmitter {
	float light_intensity;
	bool addedToLightMap = false;
};

struct FuelBurner {
	float fuel_amount;
	float burn_rate;
};

struct Gun {
	float range;
	float damage;
	float fire_rate;
	float reload_time;

	int ammo_capacity;
	int current_ammo;
};

struct NeedsMoving {};

struct Craftable {
	std::unordered_map<std::string, int> ingredients;
	std::string benchRequired;
	int quantity;
};

struct Seed {
	std::string produce;
};

struct Furniture {
	bool active;
};

struct Sittable {};
struct Table {};
struct Structure {};
struct Bed {
	bool i;
};

struct BlueprintTag {};