#include "Inventory.h"
#include "Jobs/Jobs.h"
#include "Entities/ItemComponents.h"
#include "Entities/Gun.h"
#include "Entities/CreatureComponents.h"
#include "World/World.h"

void updateInventories(entt::registry& registry, float deltaTime) {
	auto view = registry.view<JobComponent, Inventory>();

	for (auto [entity, jobComponent, inventory] : view.each()) {
		inventory.clock += deltaTime;
		if (inventory.clock < 0.1f) continue;
		inventory.clock = 0.0f;

		AmmoType type = AmmoType::UNKNOWN;
		for (auto& i : inventory.items) {
			if (auto* w = registry.try_get<Gun>(i)) {
				type = w->accepts;
				break;
			}
		}

		if (type == AmmoType::UNKNOWN && !inventory.items.empty()) {

			for (auto& i : inventory.items) {

				if (registry.all_of<PendingDrop>(i)) continue;

				auto& pos = registry.get<Position>(entity);

				auto spotOpt = mainWorld.getColony().findStockpileSpotForItem(pos.x, pos.y);
				if (!spotOpt) break;

				auto& [stockpile, stockpilePos] = *spotOpt;
				stockpile->addItem(i, stockpilePos.first, stockpilePos.second);

				Job* job = new DropEquipment(entity, entt::null, SkillType::None, stockpilePos.first, stockpilePos.second, i);
				job->priority = 25;

				if (jobComponent.willAccept(job)) {
					jobComponent.proposeJob(std::move(job));
					registry.emplace<PendingDrop>(i);
				}
				else {
					stockpile->releaseTile(stockpilePos.first, stockpilePos.second);
				}
			}
		}

		if (inventory.is_full()) continue;

		for (auto& stockpile : mainWorld.getColony().getStockpiles()) {
			for (auto& [loc, items] : stockpile.getItems()) {
				if (items.empty()) continue;

				entt::entity magEntity = items[0];

				auto* mag = registry.try_get<Magazine>(magEntity);
				if (!mag || mag->contains != type) continue;

				auto* claimable = registry.try_get<Claimable>(magEntity);
				if (claimable && claimable->claimed) continue;

				Job* job = new EquipItem(entity, entt::null, SkillType::None, magEntity);
				job->priority = 100.0f;
				if (jobComponent.willAccept(job)) {
					if (claimable) claimable->claimed = true;
					jobComponent.proposeJob(std::move(job));
					break;
				}
				else {
					delete job;
				}
			}
		}
	}
}

entt::entity hasWeapon(entt::registry& registry, entt::entity entity) {
	auto& inventory = registry.get<Inventory>(entity);

	for (auto& i : inventory.items) {
		if (auto* w = registry.try_get<Gun>(i)) {
			return i;
		}
	}

	return entt::null;
}