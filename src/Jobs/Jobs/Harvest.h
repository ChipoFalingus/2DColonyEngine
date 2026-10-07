#include "Jobs/Job.h"

class Harvest : public Job {
private:
	bool itemFound = false;
	entt::entity item;


	Harvest(entt::entity v, entt::entity tool, SkillType skillType, entt::entity i)
		: Job(v, tool, skillType), item(i)
	{}

	void update();
};