#pragma once
#include "Jobs/Job.h"

class PlaceItem : public Job {
private:
	int locX{0}, locY{0};
	entt::entity itemToPlace;

	void update();

public:
	PlaceItem(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int locX, int locY)
		: Job(v, tool, skillType), itemToPlace(item), locX(locX), locY(locY)
	{}
};