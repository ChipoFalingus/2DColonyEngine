#include "Jobs/Job.h"

class Sit : public Job {
private:
	int tX, tY;
	entt::entity chair;
	bool init = false;

	float clock = 0.0f;

	void update();
	void onInterrupt();

public:
	Sit(entt::entity v, entt::entity tool, SkillType skillType, entt::entity chair, int x, int y)
		: Job(v, tool, skillType), chair(chair), tX(x), tY(y)
	{
	}
};