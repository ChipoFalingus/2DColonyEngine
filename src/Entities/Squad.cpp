#include "Squad.h"
#include "Utility/CreatureUtils.h"
#include "Villager.h"
#include "Utility/ItemUtils.h"
#include "CreatureComponents.h"
#include "World/World.h"

void findTargets(entt::entity entity, entt::registry& registry);
SelectedTarget chooseTarget(entt::entity squad, entt::entity member, entt::registry& registry);
std::pair<int, int> getAvgPos(entt::entity entity, entt::registry& registry);
void onIdle(entt::entity squad);
void onAttack(entt::entity squad);

void updateSquadMovement(entt::entity squad) {
	auto& registry = mainWorld.registry;
	auto& squadView = registry.get<SquadController>(squad);

	findTargets(squad, registry);

	squadView.state = (squadView.targets.empty()) ? IDLE : ATTACKING;

	if (squadView.state == SquadState::IDLE) {
		onIdle(squad);
	}
	else if (squadView.state == SquadState::ATTACKING) {
		// do later

		onAttack(squad);
	}

	
}

void onIdle(entt::entity squad) {

	const int flowSize = 64;
	const int half = flowSize / 2;

	auto& registry = mainWorld.registry;
	auto& squadView = registry.get<SquadController>(squad);

	squadView.idleWanderClock += Clock::deltaTime;
	if (squadView.macroFlowField.empty() || squadView.idleWanderClock > 6.0f) {
		squadView.idleWanderClock = 0.0f;
		auto avg = getAvgPos(squad, registry);

		int range = 25;
		int randX = getRandomInt(avg.first - range, avg.first + range);
		int randY = getRandomInt(avg.second - range, avg.second + range);

		if (getTileRef(randX, randY).walkable) {
			squadView.groupTargetPos = { randX, randY };
			squadView.macroFlowField = buildFlowField(randX, randY, flowSize);
		}
	}

	if (squadView.macroFlowField.empty()) return;

	for (auto& i : squadView.members) {
		auto& pos = registry.get<Position>(i);
		auto& movable = registry.get<Movable>(i);

		movable.movementClock += Clock::deltaTime;
		if (movable.movementClock > movable.speed) {
			movable.movementClock = 0.0f;

			int localX = pos.x - (squadView.groupTargetPos.first - half);
			int localY = pos.y - (squadView.groupTargetPos.second - half);

			if (localX < 0 || localX >= flowSize || localY < 0 || localY >= flowSize) {
				continue;
			}

			auto& dir = squadView.macroFlowField[(localY * flowSize) + localX];

			int sepX = 0;
			int sepY = 0;

			for (auto other : squadView.members) {
				if (other == i) continue;
				auto& otherPos = registry.get<Position>(other);
				int dx = pos.x - otherPos.x;
				int dy = pos.y - otherPos.y;
				int dist2 = dx * dx + dy * dy;

				if (dist2 > 0 && dist2 <= 4) {
					sepX += dx;
					sepY += dy;
				}
			}

			int moveX = dir.dx;
			int moveY = dir.dy;

			int pushX = (sepX > 0) - (sepX < 0);
			int pushY = (sepY > 0) - (sepY < 0);

			moveX += pushX;
			moveY += pushY;

			moveX = std::clamp(moveX, -1, 1);
			moveY = std::clamp(moveY, -1, 1);

			int newX = pos.x + moveX;
			int newY = pos.y + moveY;

			if (!getTileRef(newX, newY).walkable) {
				newX = pos.x + dir.dx;
				newY = pos.y + dir.dy;
				if (!getTileRef(newX, newY).walkable) continue;
			}

			mainWorld.objectManager.removeItem(pos.x, pos.y, i);
			mainWorld.objectManager.addObject(newX, newY, i);

			pos.x = newX;
			pos.y = newY;
		}
	}
}

void onAttack(entt::entity squad) {
	auto& registry = mainWorld.registry;
	auto& squadView = registry.get<SquadController>(squad);

	for (auto& i : squadView.members) {
		auto& memberComponent = registry.get<SquadMemberComponent>(i);
		memberComponent.target = chooseTarget(squad, i, registry);

		auto targetEntity = memberComponent.target.target;

		if (targetEntity == entt::null) continue;

		auto& movable = registry.get<Movable>(i);
		auto& pos = registry.get<Position>(i);

		auto& targetPos = registry.get<Position>(targetEntity);

		movable.hasTarget = true;
		movable.targetX = targetPos.x;
		movable.targetY = targetPos.y;

		if (pos.x == targetPos.x && pos.y == targetPos.y) {
			auto& structureHealth = registry.get<Structure>(targetEntity);
			structureHealth.health--;
		}
	}
}

SelectedTarget chooseTarget(entt::entity squad, entt::entity member, entt::registry& registry) {
	auto& squadComponent = registry.get<SquadController>(squad);
	auto& pos = registry.get<Position>(member);

	int bestScore = 999999;
	size_t bestIndex = 0;
	bool foundValidTarget = false;

	for (size_t i = 0; i < squadComponent.targets.size(); i++) {
		auto& currentTarget = squadComponent.targets[i];

		int score = 0;
		score += currentTarget.target_population * 200;

		if (!registry.valid(currentTarget.target) || !registry.all_of<Position>(currentTarget.target)) continue;

		auto& targetPos = registry.get<Position>(currentTarget.target);

		int dx = std::abs(targetPos.x - pos.x);
		int dy = std::abs(targetPos.y - pos.y);

		score += dx + dy;
		score += currentTarget.score;

		if (score < bestScore) {
			bestIndex = i;
			bestScore = score;
			foundValidTarget = true;
		}
	}

	if (!foundValidTarget) {
		SelectedTarget nullTarget;
		nullTarget.target = entt::null;
		nullTarget.score = 0;
		nullTarget.target_population = 0;
		return nullTarget;
	}

	squadComponent.targets[bestIndex].target_population++;
	return squadComponent.targets[bestIndex];
}

void findTargets(entt::entity entity, entt::registry& registry) {
	auto& squad = registry.get<SquadController>(entity);
	squad.attackScanClock += Clock::deltaTime;

	if (squad.attackScanClock > 0.5f) {
		squad.attackScanClock = 0.0f;

		squad.targets.clear();

		auto center = getAvgPos(entity, registry);
		const int scanRadius = 32;

		auto structures = findAllItemInRange(center.first, center.second, scanRadius, [](entt::entity entity, entt::registry& registry, int x, int y) {
			return registry.all_of<Structure>(entity);
			});

		if (structures.has_value()) {
			for (auto& s : structures.value()) {
				SelectedTarget selected;
				selected.target = s.item;
				selected.score = 1;

				squad.targets.push_back(selected);
			}
		}

		auto creatures = findAllItemInRange(center.first, center.second, scanRadius, [](entt::entity entity, entt::registry& registry, int x, int y) {
			return registry.all_of<Villager>(entity);
			});

		if (creatures.has_value()) {
			for (auto& c : creatures.value()) {
				SelectedTarget selected;
				selected.target = c.item;
				selected.score = 1;

				squad.targets.push_back(selected);
			}
		}
	}
}

std::pair<int, int> getAvgPos(entt::entity entity, entt::registry& registry) {
	auto& squad = registry.get<SquadController>(entity);
	if (squad.members.empty()) return { 0, 0 };

	long long sumX = 0;
	long long sumY = 0;
	size_t valid = 0;

	for (auto member : squad.members) {
		if (registry.valid(member)) {
			auto& pos = registry.get<Position>(member);
			sumX += static_cast<long long>(pos.x);
			sumY += static_cast<long long>(pos.y);
			valid++;
		}
	}

	if (valid == 0) return { 0, 0 };

	int avgX = static_cast<int>(sumX / static_cast<long long>(valid));
	int avgY = static_cast<int>(sumY / static_cast<long long>(valid));

	return { avgX, avgY };
}

void updateSquadComponent() {
	auto view = mainWorld.registry.view<SquadController>();
	for (auto& i : view) {
		updateSquadMovement(i);
	}
}