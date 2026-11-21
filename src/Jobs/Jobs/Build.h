#pragma once
#include "Jobs/Job.h"

class Build : public Job {
private:
	std::string itemName{""};
	entt::entity staticRecipeEntity{entt::null};

	std::vector<ItemLocation> reserve;

	int locX{0}, locY{0};

	float clock{0.0f};
	float craftTime{0.0f};

	bool grabbedAllItems{ false };

	void update();
	void onWaiting();

public:
	Build(entt::entity v, entt::entity tool, SkillType skillType, std::string itemName, int locX, int locY);
};