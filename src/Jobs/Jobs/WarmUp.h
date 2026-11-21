#include "Jobs/Job.h"

class WarmUp : public Job {
private:
	entt::entity heat_source{entt::null};

	float clock{0.0f};
	bool init{false};

	void update();
	void onInterrupt();

public:
	WarmUp(entt::entity v, entt::entity tool, SkillType skillType, entt::entity heat_source)
		: Job(v, tool, skillType), heat_source(heat_source)
	{
	}
};