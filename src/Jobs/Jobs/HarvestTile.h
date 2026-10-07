#pragma once
#include "Jobs/Job.h"

class HarvestTile : public Job {
private:

	enum State {
		GrabbingTool,
		MovingToTile,
		Harvesting,
	};

	entt::entity item;
	int locX, locY;

	float harvestClock = 0.0f;

	State harvestState = State::MovingToTile;

	void update() override;

public:
	HarvestTile(entt::entity v, entt::entity tool, SkillType skill, entt::entity i, int locX, int locY)
		: Job(v, tool, skill), item(i), locX(locX), locY(locY)
	{
		x = locX;
		y = locY;
	}
};