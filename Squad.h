#pragma once
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>


#include "Creature.h"
#include "mathUtils.h"
#include "Tile.h"

class Squad {
private:

	std::pair<int, int > targetPos = {0, 0};

	std::vector<Creature*> members;

	std::pair<int, int> lastTargetPos = {0,0};

	std::vector<std::vector<std::pair<int, int>>> flow;

	float idleWanderClock;
	float attackScanClock;
public:

	enum state { IDLE, MOVING, ATTACKING } state = IDLE;


	void addMember(Creature* member);
	void update();

	void followLeader();

	void removeDeadMembers();

	std::pair<int, int> getAvgPos();

	int memberCount() {
		return members.size();
	}
};