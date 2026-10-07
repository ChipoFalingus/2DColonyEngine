#include "Jobs/Job.h"

class WarmUp : public Job {
private:
	std::optional<ItemLocation> location = std::nullopt;

	float clock = 0.0f;
	bool init = false;

	void update();
	void onInterrupt();

public:
	WarmUp(entt::entity v, entt::entity tool, SkillType skillType)
		: Job(v, tool, skillType)
	{
	}
};