#include "Jobs/Jobs/DropEquipment.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"
#include "Entities/Inventory.h"

void DropEquipment::update() {
	auto& registry = mainWorld.registry;
	auto& pos = registry.get<Position>(villager);

	std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, dropX, dropY);

	x = closestAdj.first;
	y = closestAdj.second;

	if (pos.x == x && pos.y == y) {
		auto& name = registry.get<Name>(item);
		std::cout << name.name << std::endl;


		registry.emplace_or_replace<Position>(item, dropX, dropY);
		registry.get<Claimable>(item).claimed = false;

		// add to object manager
		mainWorld.objectManager.addObject(dropX, dropY, item);

		// add item to stockpile
		if (auto* s = mainWorld.getColony().atStockpile(dropX, dropY)) {
			s->placeItem(item, dropX, dropY);
		}

		auto& inventory = registry.get<Inventory>(villager);
		inventory.remove_from_inventory(item, registry);

		registry.remove<PendingDrop>(item);

		state = JobState::Completed;
		return;
	}
}