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
		onAttack(squad);
	}

	
}

void onIdle(entt::entity squad) {
	const int flowSize = 64;
	const int half = flowSize / 2;

	auto& registry = mainWorld.registry;
	auto& squadView = registry.get<SquadController>(squad);

	squadView.idleWanderClock += Clock::deltaTime;
	if (squadView.macroFlowField.empty() || squadView.idleWanderClock > 20.0f) {
		squadView.idleWanderClock = 0.0f;
		auto avg = getAvgPos(squad, registry);

		int range = 20;
		int randX = getRandomInt(avg.first - range, avg.first + range);
		int randY = getRandomInt(avg.second - range, avg.second + range);

		if (getTileRef(randX, randY).walkable) {
			squadView.groupTargetPos = { randX, randY };
			squadView.macroFlowField = buildFlowField(randX, randY, flowSize);
		}
	}

	if (squadView.macroFlowField.empty()) return;

	for (auto& i : squadView.members) {
		if (!registry.valid(i)) continue;

		auto& pos = registry.get<Position>(i);
		auto& movable = registry.get<Movable>(i);

		int localX = (pos.x - squadView.groupTargetPos.first) + half;
		int localY = (pos.y - squadView.groupTargetPos.second) + half;

		if (localX < 0 || localX >= flowSize || localY < 0 || localY >= flowSize) {
			movable.dirX = 0;
			movable.dirY = 0;
			continue;
		}

		auto& dir = squadView.macroFlowField[(localY * flowSize) + localX];

		movable.dirX = dir.dx;
		movable.dirY = dir.dy;
	}
}

void onAttack(entt::entity squad) {
	auto& registry = mainWorld.registry;
	auto& squadView = registry.get<SquadController>(squad);

	for (auto& i : squadView.members) {
		if (!registry.valid(i)) continue;

		auto& memberComponent = registry.get<SquadMemberComponent>(i);
		memberComponent.target = chooseTarget(squad, i, registry);

		auto targetEntity = memberComponent.target.target;
		if (targetEntity == entt::null) {
            continue;
        }

		auto& movable = registry.get<Movable>(i);
		auto& pos = registry.get<Position>(i);

		auto& targetPos = registry.get<Position>(targetEntity);
		

		if (std::abs(pos.x - targetPos.x) <= 1 && std::abs(pos.y - targetPos.y) <= 1) {
			auto& health = registry.get<Health>(targetEntity);
			memberComponent.attackClock += Clock::deltaTime;
			if (memberComponent.attackClock > 0.5f) {
				memberComponent.attackClock = 0.0f;
				health.health--;
			}
		}
		else {
			memberComponent.attackClock = 0.0f;

			int dx = targetPos.x - pos.x;
			int dy = targetPos.y - pos.y;

			int moveX = (dx > 0) - (dx < 0);
			int moveY = (dy > 0) - (dy < 0);

			int newX = pos.x + moveX;
			int newY = pos.y + moveY;

			/*if (!getTileRef(newX, newY).walkable) {
				if (moveX != 0 && getTileRef(pos.x + moveX, pos.y).walkable) {
					newX = pos.x + moveX;
					newY = pos.y;
				}
				else if (moveY != 0 && getTileRef(pos.x, pos.y + moveY).walkable) {
					newX = pos.x;
					newY = pos.y + moveY;
				}
				else {
					newX = pos.x;
					newY = pos.y;
				}
			}*/

			movable.dirX = moveX;
			movable.dirY = moveY;
		}
	}
}

SelectedTarget chooseTarget(entt::entity squad, entt::entity member, entt::registry& registry) {
	auto& squadComponent = registry.get<SquadController>(squad);
	auto& pos = registry.get<Position>(member);

	auto& memberComponent = registry.get<SquadMemberComponent>(member);
	if (memberComponent.target.target != entt::null) {
		SelectedTarget nullTarget;
		nullTarget.target = entt::null;
		nullTarget.score = 0;
		nullTarget.target_population = 0;
		return nullTarget;
	};

	int bestScore = 999999;
	size_t bestIndex = 0;
	bool foundValidTarget = false;

	for (size_t i = 0; i < squadComponent.targets.size(); i++) {
		auto& currentTarget = squadComponent.targets[i];

		int score = 0;
		score += currentTarget.target_population * 200;

		if (!registry.valid(currentTarget.target) || !registry.all_of<Position>(currentTarget.target)) continue;
		if (!registry.valid(member)) continue;

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

	//squadComponent.targets[bestIndex].target_population++;
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

		auto entities = findAllItemInRange(center.first, center.second, scanRadius, [](entt::entity entity, entt::registry& registry, int x, int y) {
			return registry.all_of<Structure>(entity) || registry.all_of<Villager>(entity);
			});

		if (entities.has_value()) {
			for (auto& s : entities.value()) {
				SelectedTarget selected;
				selected.target = s.item;
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
	auto& registry = mainWorld.registry;
	auto view = registry.view<SquadController>();
	std::vector<entt::entity> empty_squads;


	for (auto& i : view) {
		updateSquadMovement(i);
		auto& controller = registry.get<SquadController>(i);
		if (controller.members.empty()) {
			empty_squads.push_back(i);
		}

		std::erase_if(controller.members, [&registry](entt::entity e) {
			auto i = registry.try_get<Health>(e);
			return !i || i->health <= 0;
			});
	}

	for (auto& i : empty_squads) {
		registry.destroy(i);
	}
}