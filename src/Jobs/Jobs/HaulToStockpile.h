#pragma once
#include "Jobs/Job.h"

class HaulToStockpile : public Job {
private:
	enum State {
		PickUpItem,
		Move,
		Drop,
	};

	State moveState{ State::PickUpItem };

	entt::entity itemToMove;
	int fromX{0}, fromY{0};
	int toX{0}, toY{0};

	void update();

public:
	HaulToStockpile(entt::entity v, entt::entity tool, SkillType skillType, entt::entity item, int fX, int fY, int tX, int tY)
		: Job(v, tool, skillType), itemToMove(item), fromX(fX), fromY(fY), toX(tX), toY(tY)
	{
		x = fX;
		y = fY;
	}
};