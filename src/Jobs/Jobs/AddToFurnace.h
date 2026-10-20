#pragma once
#include "Jobs/Job.h"

class AddToFurnace : public Job {
private:

	enum State {
		FetchingItems,
		MovingToBench,
		Crafting
	};

	State jobState{ State::FetchingItems };

	std::string item{ "" };
	entt::entity staticRecipeEntity{ entt::null };

	std::unordered_map<std::string, int> ingredients;
	std::vector<ItemLocation> reserve;

	entt::entity furnace{entt::null};

	bool init{ false };
	bool grabbedAllItems{ false };

	float clock{ 0.0f };
	float craftTime{ 0.0f };

	void update();

public:
	AddToFurnace(entt::entity v, entt::entity tool, SkillType skillType, std::string item)
		: Job(v, tool, skillType), item(item)
	{
	}
};