#pragma once
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#include "Creature.h"
#include "Structure.h"
#include "mathUtils.h"
#include "Tile.h"

struct SelectedTarget {
	std::pair<int, int> position;
	std::weak_ptr<Structure> targetStructure;
	Creature* targetCreature;
	std::vector<std::vector<std::pair<int, int>>> flow;
	int score;
	bool flowBuilt = false;

	SelectedTarget(std::pair<int, int> pos, std::weak_ptr<Structure> structure, int s)
		: position(pos), targetStructure(structure), targetCreature(nullptr), score(s), flowBuilt(false) {
	}

	SelectedTarget(std::pair<int, int> pos, Creature* creature, int s)
		: position(pos), targetStructure(), targetCreature(creature), score(s), flowBuilt(false) {
	}
};

class Squad {
private:

	std::pair<int, int> targetPos = {0, 0};

	std::vector<Creature*> members;

	std::pair<int, int> lastTargetPos = {0,0};

	std::vector<std::vector<std::pair<int, int>>> flow;

	std::vector<SelectedTarget> selectedTargets;

	float idleWanderClock;
	float attackScanClock;

	bool structureFound = false;
public:

	enum state { IDLE, MOVING, ATTACKING } state = IDLE;


	void addMember(Creature* member);
	void update();

	void followLeader();

	void removeDeadMembers();

	void findTargets();
	SelectedTarget* chooseTarget(Creature* member, std::vector<int>& targetCounts, bool allowCreatures = true);

	std::pair<int, int> getAvgPos();

	int memberCount() {
		return members.size();
	}

	std::vector<Creature*>& getMembers() {
		return members;
	}
};