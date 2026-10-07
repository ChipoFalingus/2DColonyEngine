#include "Jobs/Job.h"

class Retreat : public Job {
private:

	entt::entity threat;
	bool foundPath = false;
	bool init = false;

	void update();

public:
	Retreat(entt::entity v, entt::entity tool, SkillType skillType, entt::entity threat)
		: Job(v, tool, skillType), threat(threat)
	{
	}
};