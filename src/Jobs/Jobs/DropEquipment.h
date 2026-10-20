#pragma once
#include "Jobs/Job.h"

class DropEquipment : public Job {
private:

	entt::entity item;
	int dropX, dropY;

	bool init = false;

	void update();

public:
	DropEquipment(entt::entity v, entt::entity tool, SkillType skillType, int dropX, int dropY, entt::entity item)
		: Job(v, tool, skillType), dropX(dropX), dropY(dropY), item(item)
	{
	}
};