#include "Attack.h"
#include "World/World.h"

void Attack::update() {
	attackClock += Clock::deltaTime;
	rescanClock += Clock::deltaTime;

	auto& attackComponent = mainWorld.registry.get<CombatComponent>(villager);
	auto* equipment = mainWorld.registry.try_get<Equipment>(villager);

	// default stats
	float range = 1.0f;
	float attackCooldown = 0.5f;
	int dmg = 5;

	if (equipment) {
		if (auto gun = mainWorld.registry.try_get<Gun>(equipment->item)) {
			if (gun->canFire()) {
				range = gun->range;
				attackCooldown = gun->fire_rate;
				dmg = gun->damage;
			}
		}
	}

	auto& pos = mainWorld.registry.get<Position>(villager);

	if (rescanClock > 0.5f || !mainWorld.registry.valid(target)) {
		rescanClock = 0.0f;
		auto newThreat = findClosestItemType(pos.x, pos.y, 25, [&](entt::entity entity, entt::registry& reg, int x, int y) {
			return reg.try_get<Hostile>(entity) && (entity != villager);
			});

		if (newThreat.has_value()) {
			target = newThreat.value().item;
		}
		else {
			if (auto movable = mainWorld.registry.try_get<Movable>(villager)) {
				movable->hasTarget = false;
			}
			state = JobState::Completed;
			return;
		}
	}

	auto& targetPos = mainWorld.registry.get<Position>(target);

	float dx = static_cast<float>(targetPos.x - pos.x);
	float dy = static_cast<float>(targetPos.y - pos.y);
	float distSq = dx * dx + dy * dy;
	float rangeSq = range * range;

	auto& movable = mainWorld.registry.get<Movable>(villager);

	if (distSq > rangeSq) {
		movable.hasTarget = true;
		x = targetPos.x;
		y = targetPos.y;
	}
	else {
		// Stay in place
		movable.hasTarget = false;
		movable.path.clear();
		x = pos.x;
		y = pos.y;

		auto line = bresenham(pos.x, pos.y, targetPos.x, targetPos.y);

		if (attackClock > attackCooldown) {
			attackClock -= attackCooldown;

			if (auto* gun = mainWorld.registry.try_get<Gun>(equipment->item)) {
				if (gun->canFire()) {
					gun->shoot();
				}
			}

			if (auto health = mainWorld.registry.try_get<Health>(target)) {
				health->health -= dmg;
			}


			for (int i = 1; i < line.size() - 1; i++) {
				getTileRef(line[i].first, line[i].second).setAnimType(GUN_SHOT);
			}
		}
	}
}