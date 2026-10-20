#pragma once
#include "Jobs/Job.h"

class EquipItem : public Job {
private:
	entt::entity item{entt::null};

	void update();
public:
	EquipItem(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item)
		: Job(v, tool, skillType), item(item)
	{
	}
};