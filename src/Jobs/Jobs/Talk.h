#pragma once
#include "Jobs/Job.h"

class Talk : public Job {
private:
	enum State {
		WalkTo,
		TalkTo
	};

	State talkState{State::WalkTo};

	entt::entity other;
	float clock{0.0f};

	void update();

public:
	Talk(entt::entity v, entt::entity tool, SkillType skillType, entt::entity other)
		: Job(v, tool, skillType), other(other)
	{
	}
};