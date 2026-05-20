#include "Squad.h"
#include "CreatureUtils.h"
#include "Villager.h"

void Squad::addMember(Creature* member) {
	members.push_back(member);
}

void Squad::update() {
	removeDeadMembers();

	idleWanderClock += Clock::deltaTime;
	attackScanClock += Clock::deltaTime;

	switch (state) {
	case IDLE: {

		auto avg = getAvgPos();
		int range = 5;

		if (idleWanderClock > 1.0f) {
			idleWanderClock = 0.0f;

			targetPos = {
				getRandomInt(avg.first - range, avg.first + range),
				getRandomInt(avg.second - range, avg.second + range)
				//avg.first,
				//avg.second
			};
		}

		if (attackScanClock > 2.0f) {
			attackScanClock = 0.0f;

			/*Creature* c = findClosestCreatureType<Villager>(
				avg.first,
				avg.second,
				64
			);

			if (c) {
				state = state::ATTACKING;
				targetPos = { c->xPos, c->yPos };
			}*/
		}

		break;
	}

	case ATTACKING:

		if (attackScanClock > 0.5f) {
			attackScanClock = 0.0f;
			
			Creature* c = findClosestCreatureType<Villager>(
				getAvgPos().first,
				getAvgPos().second,
				96
			);

			if (c) {
				targetPos = { c->xPos, c->yPos };
			}
			else {
				state = state::IDLE;
			}
		}
		break;
	}


	followLeader();
}

void Squad::followLeader() {

	const int dim = 32;
	const int half = dim / 2;

	auto center = getAvgPos();

	if (flow.empty() || abs(targetPos.first - lastTargetPos.first) +
		abs(targetPos.second - lastTargetPos.second) > 1) {

		flow = buildFlowField(targetPos.first, targetPos.second, dim);
		lastTargetPos = { targetPos.first, targetPos.second };
	}

	if (flow.empty()) {
		return;
	}


	int startX = targetPos.first - half;
	int startY = targetPos.second - half;
	

	for (Creature* member : members) {
		
		int fx = member->xPos - startX;
		int fy = member->yPos - startY;

		if (fx < 0 || fy < 0 || fx >= dim || fy >= dim)
			continue;

		auto dir = flow[fx][fy];

		if (dir.first == 0 && dir.second == 0)
			continue;

		int sepX = 0;
		int sepY = 0;

		for (Creature* other : members) {
			if (other == member) continue;

			int dx = member->xPos - other->xPos;
			int dy = member->yPos - other->yPos;

			int dist2 = dx * dx + dy * dy;

			if (dist2 > 0 && dist2 <= 4) {
				sepX += dx;
				sepY += dy;
			}
		}

		int moveX = dir.first;
		int moveY = dir.second;

		moveX += (sepX > 0) - (sepX < 0);
		moveY += (sepY > 0) - (sepY < 0);

		moveX = std::clamp(moveX, -1, 1);
		moveY = std::clamp(moveY, -1, 1);

		int newX = member->xPos + moveX;
		int newY = member->yPos + moveY;

		if (!getTileRef(newX, newY).walkable)
			continue;

		if (member->clock.getElapsedTime().asSeconds() > member->speed) {
			member->xPos = newX;
			member->yPos = newY;
			member->clock.restart();
		}
		
	}
}

void Squad::removeDeadMembers() {
	members.erase(
		std::remove_if(members.begin(), members.end(), [](Creature* c) { return c->dead; }),
		members.end()
	);
}

std::pair<int, int> Squad::getAvgPos() {
	if (members.empty()) return { 0, 0 };

	long long sumX = 0;
	long long sumY = 0;
	size_t valid = 0;

	for (Creature* member : members) {
		if (!member) {
			continue;
		}
		if (member->dead) {
			continue;
		}

		sumX += static_cast<long long>(member->xPos);
		sumY += static_cast<long long>(member->yPos);
		++valid;
	}

	if (valid == 0) return { 0, 0 };

	int avgX = static_cast<int>(sumX / static_cast<long long>(valid));
	int avgY = static_cast<int>(sumY / static_cast<long long>(valid));

	return { avgX, avgY };
}