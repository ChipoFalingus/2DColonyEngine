#pragma once
#include "Jobs/Job.h"

class Guard : public Job {
private:
	entt::entity currentPost{ entt::null };

	float clock{ 0.0f };
	float searchClock{ 0.0f };
	bool init{ false };

	void update();
	void onInterrupt();

public:
	Guard(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{
	}
};