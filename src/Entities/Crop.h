#pragma once
#include <string>
#include <entt/entt.hpp>

enum class CropState {
	SPROUT,
	MATURED,
	DEAD
};

struct CropStage {
	wchar_t character;
	glm::vec3 color;
};

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

class CropManager {
private:
    entt::registry& registry;
public:
    CropManager(entt::registry& reg) : registry(reg) {}

    void updateCropSystem(float deltaTime);
};