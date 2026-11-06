#pragma once
#include "Jobs/Job.h"

class Guard : public Job {
private:
	entt::entity postToIgnore{ entt::null };

	float clock{ 0.0f };
	float searchClock{ 0.0f };
	bool init{ false };

	void update();

public:
	Guard(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{
	}
};