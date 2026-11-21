#pragma once
#include "Jobs/Job.h"

class Combat : public Job {
private:

	enum Action {
		Idle,
		Attack,
		Retreat,
		Retreat_s,
		Get_Ammo
	};

	Action combatState{ Action::Idle};
	
	entt::entity target{entt::null};
	entt::entity weapon{entt::null};
	entt::entity found_stuff{entt::null};

	float attackClock{0.0f};
	float rescanClock{0.0f};
	float decideClock{0.0f};

	bool foundPath = false;
	bool init = false;

	void update();

	void updateAttack();
	void updateRetreat();
	void updateGetSupplies();

	float scoreAttack(entt::registry& registry);
	float scoreFullRetreat(entt::registry& registry);
	float scoreHalfRetreat(entt::registry& registry);
	float scoreGetSupplies(entt::registry& registry);

public:
	Combat(entt::entity v, entt::entity tool, SkillType skillType, entt::entity target)
		: Job(v, tool, skillType), target(target)
	{
	}
};