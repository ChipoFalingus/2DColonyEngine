#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Jobs/JobType.h"
#include "World/Colony.h"

// how many structs could a guy need?

// probably need to split this up into different files because 1 change makes recompilation take an agonizing amount of time

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

// things that can be picked up with no harvesting
struct Pickupable {};

struct Health {
	int health;
};

struct Nutritional {
	int nutrition;
};

struct ProduceSpawner {
	std::string produce;
	float productionTime;
	float produceClock;

	int range = 1;

	bool isProducing = false;
	bool canProduce = true;
};

struct FirePitSlot {
	int x, y;
	entt::entity entity;
};

struct FirePitComponent {
	std::vector<FirePitSlot> slots;
};

struct HeatEmitter {
	float heat_intensity;
	bool addedToHeatMap = false;
};

struct LightEmitter {
	float light_intensity;
	bool addedToLightMap = false;
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

struct Bed {};
struct Floor{};

struct BlueprintTag {};

// add colony variable
struct NeedsMoving {
	bool foundSpot = false;
};

struct PendingDrop {};

struct Equipped {};

struct BelongsTo {
	Colony& owner;
};

struct GuardPost {
	bool active = false;
};

struct Desconstructable {};