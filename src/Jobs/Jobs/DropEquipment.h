#pragma once
#include "Jobs/Job.h"

class DropEquipment : public Job {
private:

	entt::entity item{ entt::null };
	int dropX{ 0 };
	int dropY{ 0 };

	bool init{ false };

	void update();

public:
	DropEquipment(entt::entity v, entt::entity tool, SkillType skillType, int dropX, int dropY, entt::entity item)
		: Job(v, tool, skillType), dropX(dropX), dropY(dropY), item(item)
	{
	}
};