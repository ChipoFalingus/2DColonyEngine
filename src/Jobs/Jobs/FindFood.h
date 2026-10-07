#pragma once
#include "Jobs/Job.h"

class FindFood : public Job {
private:

	enum State {
		Find,
		Grab,
		Eat,
	};

	State foodState{State::Grab};
	std::optional<ItemLocation> place{std::nullopt};

	entt::entity food;

	float eatTimer{0.0f};
	int tX{0}, tY{0};

	void update();

public:
	FindFood(entt::entity v, entt::entity tool, SkillType skillType, int x, int y, entt::entity food)
		: Job(v, tool, skillType), tX(x), tY(y), food(food)
	{
	}
};