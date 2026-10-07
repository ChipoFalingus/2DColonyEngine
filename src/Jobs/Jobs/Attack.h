#include "Jobs/Job.h"

class Attack : public Job {
private:

	entt::entity target;
	float attackClock = 0.0f;
	float rescanClock = 0.0f;

	void update();

public:
	Attack(entt::entity v, entt::entity tool, SkillType skillType, entt::entity target)
		: Job(v, tool, skillType), target(target)
	{
	}
};