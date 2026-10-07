#include "Jobs/Job.h"

class Idle : public Job {
private:

	float waitTime = 0.0f;
	float clock = 0.0f;
	bool initialized = false;

	void update();
	void onInterrupt();

public:
	Idle(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{
	}
};