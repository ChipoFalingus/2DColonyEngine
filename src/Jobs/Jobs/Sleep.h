#include "Jobs/Job.h"

class Sleep : public Job {
private:
	bool sleeping = false;
	bool lookedForBed = false;

	void update();

public:
	Sleep(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{
	}
};