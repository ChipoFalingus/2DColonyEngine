#include "Jobs/Job.h"	

class Nap : public Job {
private:
	float clock = 0.0f;
	bool init = false;

	void update();
	void onInterrupt();

public:
	Nap(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{
	}
};