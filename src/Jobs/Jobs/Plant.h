#pragma once
#include "Jobs/Job.h"

class Plant : public Job {
private:

	enum State {
		GetSeed,
		Move,
		Tilling,
		Planting
	};

	State plantState{State::GetSeed};

	ItemLocation itemLocation;
	std::string seed;

	int locX{0}, locY{0};

	float plantClock{0.0f};
	float tillClock{0.0f};

	void update();

public:
	Plant(entt::entity v, entt::entity tool, SkillType skillType, std::string seed, ItemLocation itemLocation, int locX, int locY)
		: Job(v, tool, skillType), seed(seed), itemLocation(itemLocation), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
	}

};