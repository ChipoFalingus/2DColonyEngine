#include "Jobs/Jobs/EquipItem.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"
#include "Entities/Inventory.h"

void EquipItem::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& itemPos = mainWorld.registry.get<Position>(item);

	auto adj = findClosestAdjTile(pos.x, pos.y, itemPos.x, itemPos.y);

	x = adj.first;
	y = adj.second;

	if (pos.x == x && pos.y == y) {
		if (auto* s = mainWorld.getColony().atStockpile(itemPos.x, itemPos.y)) {
			s->removeItem(itemPos.x, itemPos.y, item);
		}

		//mainWorld.registry.emplace<Equipment>(villager, item);

		auto& inventory = mainWorld.registry.get<Inventory>(villager);

		if (!inventory.add_to_inventory(item, mainWorld.registry)) {
			state = JobState::Completed;
			return;
		}

		mainWorld.objectManager.removeItem(itemPos.x, itemPos.y, item);
		mainWorld.registry.remove<Position>(item);

		std::cout << "Villager " << int(villager) << " added item " << int(item) << " to inventory" << std::endl;
		state = JobState::Completed;
	}
}

