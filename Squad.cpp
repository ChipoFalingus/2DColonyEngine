#include "Squad.h"
#include "CreatureUtils.h"
#include "Villager.h"

void Squad::addMember(Creature* member) {
	members.push_back(member);
}

void Squad::update() {

	removeDeadMembers();
	findTargets();

	auto avg = getAvgPos();

	idleWanderClock += Clock::deltaTime;
	attackScanClock += Clock::deltaTime;

	switch (state) {
	case IDLE: {
		if (idleWanderClock > 1.0f) {
			idleWanderClock = 0.0f;

			int range = 5;

			targetPos = {
				getRandomInt(avg.first - range, avg.first + range),
				getRandomInt(avg.second - range, avg.second + range)
				//avg.first,
				//avg.second
			};
		}

		if (attackScanClock > 2.0f) {
			attackScanClock = 0.0f;

			Creature* c = findClosestCreatureType<Villager>(
				avg.first,
				avg.second,
				64
			);

			if (c) {
				state = ATTACKING;
				//targetPos = { c->xPos, c->yPos };
			}
		}

		break;
	}

	case ATTACKING:

		if (attackScanClock > 0.5f) {
			//attackScanClock = 0.0f;
			//
			//Creature* c = findClosestCreatureType<Villager>(
			//	getAvgPos().first,
			//	getAvgPos().second,
			//	96
			//);

			//if (c) {
			//	//targetPos = { c->xPos, c->yPos };
			//}
			//else {
			//	//state = state::IDLE;
			//}
		}
		break;
	}


	followLeader();
}

void Squad::followLeader() {
	if (members.empty()) return;

	const int MACRO_DIM = 64;
	const int MICRO_DIM = 16;
	const int macroHalf = MACRO_DIM / 2;
	const int microHalf = MICRO_DIM / 2;

	auto center = getAvgPos();

	if (flow.empty() || abs(targetPos.first - lastTargetPos.first) +
		abs(targetPos.second - lastTargetPos.second) > 2) {

		flow = buildFlowField(targetPos.first, targetPos.second, MACRO_DIM);
		lastTargetPos = { targetPos.first, targetPos.second };
	}

	if (flow.empty()) {
		return;
	}

	int macroStartX = targetPos.first - macroHalf;
	int macroStartY = targetPos.second - macroHalf;

	std::vector<int> targetCounts(selectedTargets.size(), 0);

	for (Creature* member : members) {

		auto target = chooseTarget(member, targetCounts);

		std::pair<int, int> memberTargetPos = targetPos;
		std::shared_ptr<Structure> targetStructureInstance = nullptr;
		Creature* targetCreatureInstance = nullptr;

		if (target != nullptr) {
			if (target->targetCreature) {
				target->position = { target->targetCreature->xPos, target->targetCreature->yPos };
			}

			memberTargetPos = target->position;
			targetStructureInstance = target->targetStructure.lock();
			targetCreatureInstance = target->targetCreature;

			bool instance = targetCreatureInstance || targetStructureInstance;

			if (instance && !target->flowBuilt) {
				target->flow = buildFlowField(target->position.first, target->position.second, MICRO_DIM);
				target->flowBuilt = true;
			}
		}

		std::pair<int, int> dir = { 0,0 };
		bool microGridAttempted = false;

		if (target != nullptr && !target->flow.empty()) {
			int localFx = member->xPos - target->position.first;
			int localFy = member->yPos - target->position.second;
			int distToStructureSq = localFx * localFx + localFy * localFy;

			if (distToStructureSq <= (microHalf * microHalf)) {
				microGridAttempted = true;
				int microStartX = target->position.first - microHalf;
				int microStartY = target->position.second - microHalf;

				int fxMicro = member->xPos - microStartX;
				int fyMicro = member->yPos - microStartY;

				if (fxMicro >= 0 && fyMicro >= 0 && fxMicro < MICRO_DIM && fyMicro < MICRO_DIM) {
					dir = target->flow[fxMicro][fyMicro];
				}
			}
		}

		if (dir.first == 0 && dir.second == 0) {
			int fxMacro = member->xPos - macroStartX;
			int fyMacro = member->yPos - macroStartY;

			if (fxMacro >= 0 && fyMacro >= 0 && fxMacro < MACRO_DIM && fyMacro < MACRO_DIM) {
				dir = flow[fxMacro][fyMacro];
			}
		}

		if (targetStructureInstance || targetCreatureInstance) {
			int distToTargetX = abs(member->xPos - memberTargetPos.first);
			int distToTargetY = abs(member->yPos - memberTargetPos.second);

			if (distToTargetX + distToTargetY == 1) {
				if (member->clock.getElapsedTime().asSeconds() > member->speed) {

					if (targetStructureInstance) {
						targetStructureInstance->takeDamage(10);
					}
					else if (targetCreatureInstance) {
						targetCreatureInstance->takeDamage(5, member);
					}

					member->clock.restart();
				}
				continue;
			}
		}

		int dirX = dir.first;
		int dirY = dir.second;

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

		int moveX = dirX + ((sepX > 0) - (sepX < 0));
		int moveY = dirY + ((sepY > 0) - (sepY < 0));

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
		valid++;
	}

	if (valid == 0) return { 0, 0 };

	int avgX = static_cast<int>(sumX / static_cast<long long>(valid));
	int avgY = static_cast<int>(sumY / static_cast<long long>(valid));

	return { avgX, avgY };
}

void Squad::findTargets() {
	if (state == IDLE || state == ATTACKING) {
		if (attackScanClock > 0.5f) {
			attackScanClock = 0.0f;

			selectedTargets.clear();

			auto center = getAvgPos();
			const int scanRadius = 32;

			auto structures = findAllItemInRange(center.first, center.second, scanRadius, [](const Object& item, int x, int y) {
				return item.type == Type::Structure;
				});

			if (structures.has_value()) {
				for (auto& s : structures.value()) {
					auto lockedItem = s.item.lock();
					if (!lockedItem) continue;

					auto structureShared = std::static_pointer_cast<Structure>(lockedItem);
					selectedTargets.push_back(SelectedTarget({ s.x, s.y }, structureShared, 1));
				}
			}

			auto creatures = findAllCreaturesInRange<Villager>(center.first, center.second, scanRadius);
			for (auto& c : creatures) {
				if (c->dead) continue;
				selectedTargets.push_back(SelectedTarget({ c->xPos, c->yPos }, c, 1));
			}

			if (!selectedTargets.empty()) {
				state = ATTACKING;
			}
			else {
				if (state == ATTACKING) {
					flow.clear();
					idleWanderClock = 2.0f;
				}
				state = IDLE;
			}
		}
	}
}

SelectedTarget* Squad::chooseTarget(Creature* member, std::vector<int>& targetCounts, bool allowCreatures) {
	SelectedTarget* target = nullptr;
	int bestScore = 9999999;
	int chosenIndex = -1;

	if (state == ATTACKING && !selectedTargets.empty()) {

		for (size_t i = 0; i < selectedTargets.size(); i++) {
			auto& potential = selectedTargets[i];

			if (potential.targetCreature && potential.targetCreature->dead) continue;

			std::shared_ptr<Structure> lockedStruct = potential.targetStructure.lock();
			if (!potential.targetCreature && (!lockedStruct || lockedStruct->health <= 0)) continue;

			if (potential.targetCreature && !allowCreatures) continue;


			int dx = member->xPos - potential.position.first;
			int dy = member->yPos - potential.position.second;
			int dist2 = dx * dx + dy * dy;
			if (dist2 < 1) dist2 = 1;
			potential.score = dist2;

			int crowdPenalty = targetCounts[i] * 50;
			int adjustedScore = potential.score + crowdPenalty;

			if (potential.targetCreature) {
				adjustedScore -= 300;
			}

			if (adjustedScore < bestScore) {
				bestScore = adjustedScore;
				chosenIndex = static_cast<int>(i);
			}
		}

		if (chosenIndex != -1) {
			targetCounts[chosenIndex]++;
			return &selectedTargets[chosenIndex];
		}
	}

	return nullptr;
}


/*

THE ALGORITHM:


	- Put all possible targets into arrays (already doing that)
	- Add a score for each that takes distance and attention into account
	- Create a "general" flow field that directs to a nearby area
	- Create a smaller flow field (or just a*) to the actual target
	- I think the smaller flow field should ignore separation to allow higher damage without crowding


*/