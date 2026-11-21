#pragma once
#include "Jobs/Job.h"

class Deconstruct : public Job {
private:
	entt::entity item{entt::null};
	float clock{ 0.0f };

	void update();

public:
	Deconstruct(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item) :
		Job(v, tool, skillType), item(item) {
	}
};