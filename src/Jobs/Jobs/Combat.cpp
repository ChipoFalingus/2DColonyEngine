#include "Combat.h"
#include "World/World.h"
#include "Entities/Happiness.h"
#include "Entities/Inventory.h"
#include "Entities/Gun.h"
#include "Entities/Movement.h"

entt::entity findBestMagazine(entt::registry& registry, Inventory& inventory, AmmoType requiredType) {
	entt::entity bestMag = entt::null;
	int highestRounds = 0;

	for (auto& i : inventory.items) {
		if (auto* mag = registry.try_get<Magazine>(i)) {
			if (mag->contains == requiredType && mag->current_capacity > highestRounds) {
				highestRounds = mag->current_capacity;
				bestMag = i;
			}
		}
	}

	return bestMag;
}

bool swapMagazine(entt::registry& registry, entt::entity villager, Gun& gun) {
	auto& inventory = registry.get<Inventory>(villager);

	entt::entity newMag = findBestMagazine(registry, inventory, gun.accepts);
	if (newMag == entt::null) {
		return false;
	}

	auto it = std::find(inventory.items.begin(), inventory.items.end(), newMag);
	if (it != inventory.items.end()) {
		inventory.items.erase(it);
	}

	if (gun.loaded_mag != entt::null && registry.valid(gun.loaded_mag)) {
		auto* oldMag = registry.try_get<Magazine>(gun.loaded_mag);
		if (oldMag && oldMag->current_capacity > 0) {
			inventory.items.push_back(gun.loaded_mag);
		}
		else {
			registry.destroy(gun.loaded_mag);
		}
	}

	gun.loaded_mag = newMag;
	return true;
}

void reload(entt::entity weapon, entt::entity villager, entt::registry& registry) {
	auto* gun = registry.try_get<Gun>(weapon);

	gun->reload_clock += Clock::deltaTime;
	if (gun->reload_clock >= gun->reload_time) {
		gun->reloading = false;
		gun->reload_clock = 0.0f;
		swapMagazine(registry, villager, *gun);
	}
}

float Combat::scoreAttack(entt::registry& registry) {

	float score = 0.0f;
	if (weapon == entt::null) return 0.0f;
	if (registry.try_get<OnGuard>(villager)) score += 0.2f;

	auto& gun = registry.get<Gun>(weapon);
	if (gun.isEmpty(registry)) return 0.0f;

	auto* health = registry.try_get<Health>(villager);
	float healthFactor = health ? static_cast<float>(health->health) / 100.0f : 1.0f;

	//return std::clamp(healthFactor + score, 0.0f, 1.0f);
	return 0.5f;
}

float Combat::scoreFullRetreat(entt::registry& registry) {
	if (!registry.valid(target)) return 0.0f;

	float score = 0.0f;
	if (weapon == entt::null) score += 0.4f;

	auto* health = registry.try_get<Health>(villager);
	float healthFactor = health ? static_cast<float>(health->health) / 100.0f : 1.0f;

	auto& v_pos = registry.get<Position>(villager);
	auto& e_pos = registry.get<Position>(target);

	int dist = std::abs(v_pos.x - e_pos.x) + std::abs(v_pos.y - e_pos.y);

	float proximityUrgency = (dist < 5) ? (1.0f - (static_cast<float>(dist) / 5.0f)) : 0.0f;

	return score + proximityUrgency;
}

float Combat::scoreHalfRetreat(entt::registry& registry) {

	float score = 0.0f;
	return score;
}

float Combat::scoreGetSupplies(entt::registry& registry) {
	auto& inventory = registry.get<Inventory>(villager);
	if (inventory.is_full()) return 0.0f;
	if (weapon == entt::null) return 0.0f;

	for (auto& stockpile : mainWorld.getColony().getStockpiles()) {
		for (auto& [loc, items] : stockpile.getItems()) {
			if (items.empty()) continue;

			entt::entity magEntity = items[0];

			auto& gun = mainWorld.registry.get<Gun>(weapon);
			auto* mag = registry.try_get<Magazine>(magEntity);
			if (!mag || mag->contains != gun.accepts) continue;

			auto* claimable = registry.try_get<Claimable>(magEntity);
			if (claimable && claimable->claimed) continue;

			found_stuff = magEntity;
			break;
		}
	}

	if (found_stuff == entt::null) return 0.0f;

	float openSlots = inventory.items.size() / 8;
	return openSlots;
}

void Combat::update() {
	auto& registry = mainWorld.registry;
	auto& inventory = registry.get<Inventory>(villager);
	for (auto& i : inventory.items) {
		if (auto* w = registry.try_get<Gun>(i)) {
			weapon = i;

			break;
		}
	}

	if (weapon != entt::null) {
		auto& gun = registry.get<Gun>(weapon);
		if (gun.isEmpty(registry)) {
			gun.reloading = true;
			reload(weapon, villager, registry);
		}
	}


	decideClock += Clock::deltaTime;
	//if (decideClock > 1.0f) {
		decideClock = 0.0f;

		float score_attack = scoreAttack(registry);
		float score_retreat = scoreFullRetreat(registry);
		float score_ammo = scoreGetSupplies(registry);

		float bestScore = -1.0f;
		combatState = Action::Idle;

		if (score_attack > bestScore) { bestScore = score_attack; combatState = Action::Attack; }
		if (score_retreat > bestScore) { bestScore = score_retreat;   combatState = Action::Retreat; }
		if (score_ammo > bestScore) { bestScore = score_retreat;   combatState = Action::Get_Ammo; }


		/*std::cout << "==================\n"
			<< "Attack Score: " << score_attack << "\n"
			<< "Retreat Score: " << score_retreat << "\n"
			<< "Ammo Score: " << score_ammo << "\n"
			<< "==================\n"
			<< std::endl;*/
	//}


	switch (combatState) {
	case(Action::Attack):
		updateAttack();
		break;
	case(Action::Retreat):
		updateRetreat();
		break;
	case(Action::Get_Ammo):
		updateGetSupplies();
		break;
	default:
		state = JobState::Completed;
		return;
	}

}

void Combat::updateAttack() {
	auto& registry = mainWorld.registry;

	rescanClock += Clock::deltaTime;

	auto& attackComponent = registry.get<CombatComponent>(villager);
	auto& inventory = registry.get<Inventory>(villager);

	// default stats
	float range = 1.0f;
	float attackCooldown = 0.5f;
	int dmg = 5;

	if (weapon != entt::null) {
		if (auto* w = registry.try_get<Gun>(weapon)) {
			range = w->range;
			attackCooldown = w->fire_rate;
			dmg = w->damage;
		}
	}

	auto& pos = registry.get<Position>(villager);

	if (rescanClock > 0.5f || !registry.valid(target)) {
		rescanClock = 0.0f;
		auto newThreat = findClosestItemType(pos.x, pos.y, 25, [&](entt::entity entity, entt::registry& reg, int x, int y) {
			return reg.try_get<Hostile>(entity) && (entity != villager);
			});

		if (newThreat.has_value()) {
			target = newThreat.value().item;
		}
		else {
			if (auto movable = registry.try_get<Movable>(villager)) {
				movable->hasTarget = false;
			}
			state = JobState::Completed;
			return;
		}
	}

	auto& targetPos = registry.get<Position>(target);

	float dx = static_cast<float>(targetPos.x - pos.x);
	float dy = static_cast<float>(targetPos.y - pos.y);
	float distSq = dx * dx + dy * dy;
	float rangeSq = range * range;

	auto& movable = registry.get<Movable>(villager);

	if (distSq > rangeSq) {
		movable.hasTarget = true;
		x = targetPos.x;
		y = targetPos.y;

		return;
	}

	// Stay in place
	movable.hasTarget = false;
	movable.path.clear();
	x = pos.x;
	y = pos.y;

	if (weapon != entt::null) {
		if (auto* gun = registry.try_get<Gun>(weapon)) {
			if (gun->reloading) {
				return;
			}
		}
	}

	attackClock += Clock::deltaTime;

	auto line = bresenham(pos.x, pos.y, targetPos.x, targetPos.y);

	if (attackClock > attackCooldown) {
		attackClock -= attackCooldown;

		if (weapon != entt::null) {
			if (auto* gun = registry.try_get<Gun>(weapon)) {
				if (gun->canFire(registry)) {
					gun->shoot(registry);

					if (auto health = registry.try_get<Health>(target)) {
						health->health -= dmg;
					}

					for (int i = 1; i < line.size() - 1; i++) {
						getTileRef(line[i].first, line[i].second).setAnimType(GUN_SHOT);
					}
				}
				else {
					if (findBestMagazine(registry, inventory, gun->accepts) != entt::null) {
						gun->reloading = true;
						gun->reload_clock = 0.0f;
					}
					else {
						std::cout << "Out of ammo and no spare magazines! Retreating or switching to melee.\n";
					}
				}
			}
		}
	}
}

void Combat::updateRetreat() {
	auto& registry = mainWorld.registry;

	auto& pos = registry.get<Position>(villager);
	auto& movable = registry.get<Movable>(villager);

	auto& colony = mainWorld.getColony();

	rescanClock += Clock::deltaTime;

	if (!init) {
		auto& jobComponent = registry.get<JobComponent>(villager);
		jobComponent.panicClock = 0.0f;
		jobComponent.panicDuration = 10.0f;

		jobComponent.clearInterruptedJobs();

		auto& happiness = registry.get<Happiness>(villager);
		auto& name = registry.get<Name>(villager);

		init = true;
	}

	if (!foundPath) {
		int dim = 32;
		int half = dim / 2;
		std::vector<std::vector<float>> threatMap = buildThreatMap(pos.x, pos.y, dim);

		float bestScore = std::numeric_limits<float>::max();
		std::pair<int, int> bestLocation = { pos.x, pos.y };

		for (int i = -half; i < half; i++) {
			for (int j = -half; j < half; j++) {
				int worldX = pos.x + i;
				int worldY = pos.y + j;

				int retreatCost = colony.getRetreatValue(worldX, worldY);

				int localX = i + half;
				int localY = j + half;

				if (!getTileRef(worldX, worldY).walkable) {
					continue;
				}

				auto& threat_pos = registry.get<Position>(target);

				int distFromSelf = std::abs(i) + std::abs(j);
				int distFromThreat = std::abs(worldX - threat_pos.x) + std::abs(worldY - threat_pos.y);

				float score = threatMap[localX][localY] + retreatCost + distFromSelf * 0.05f + (-distFromThreat * 0.15f);

				// lower is better
				if (score < bestScore) {
					bestScore = score;
					bestLocation = { worldX, worldY };
				}
			}
		}

		x = bestLocation.first;
		y = bestLocation.second;

		foundPath = true;
	}

	if (rescanClock > 0.2f && pos.x == x && pos.y == y) {
		rescanClock = 0.0f;
		auto closestThreat = findClosestItemType(pos.x, pos.y, 20, [&](entt::entity entity, entt::registry& reg, int x, int y) {
			return reg.try_get<Hostile>(entity) && (entity != villager);
			});

		if (closestThreat.has_value()) {
			foundPath = false;
		}
		else if (pos.x == x && pos.y == y) {
			state = JobState::Completed;
		}
	}
}

void Combat::updateGetSupplies() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& itemPos = mainWorld.registry.get<Position>(found_stuff);

	auto adj = findClosestAdjTile(pos.x, pos.y, itemPos.x, itemPos.y);

	x = adj.first;
	y = adj.second;

	if (pos.x == x && pos.y == y) {
		if (auto* s = mainWorld.getColony().atStockpile(itemPos.x, itemPos.y)) {
			s->removeItem(itemPos.x, itemPos.y, found_stuff);
		}

		//mainWorld.registry.emplace<Equipment>(villager, item);

		auto& inventory = mainWorld.registry.get<Inventory>(villager);

		if (!inventory.add_to_inventory(found_stuff, mainWorld.registry)) {
			state = JobState::Completed;
			return;
		}

		mainWorld.objectManager.removeItem(itemPos.x, itemPos.y, found_stuff);
		mainWorld.registry.remove<Position>(found_stuff);

		std::cout << "Villager " << int(villager) << " added item " << int(found_stuff) << " to inventory" << std::endl;

		found_stuff = entt::null;
		state = JobState::Completed;
	}
}