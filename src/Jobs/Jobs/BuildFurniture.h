#pragma once
#include "Jobs/Job.h"

class BuildFurniture : public Job {
private:

	enum State {
		Getting,
		Placing
	};

	State jobState{ State::Getting };

	entt::entity item{};

	int fX{0}, fY{0};
	int tX{0}, tY{0};

	bool grabbedItem{false};
	bool init{false};

	void update();

public:
	BuildFurniture(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int fX, int fY, int tX, int tY)
		: Job(v, tool, skillType), item(item), fX(fX), fY(fY), tX(tX), tY(tY)
	{
		x = fX;
		y = fY;
	}
};