#include "Jobs/Jobs/EquipItem.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void EquipItem::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& itemPos = mainWorld.registry.get<Position>(item);

	auto adj = findClosestAdjTile(pos.x, pos.y, itemPos.x, itemPos.y);

	x = adj.first;
	y = adj.second;

	if (pos.x == x && pos.y == y) {
		mainWorld.registry.emplace<Equipment>(villager, item);

		mainWorld.objectManager.removeItem(itemPos.x, itemPos.y, item);
		if (mainWorld.registry.any_of<Position>(item)) {
			mainWorld.registry.remove<Position>(item);
		}

		if (auto* s = mainWorld.atStockpile(itemPos.x, itemPos.y)) {
			s->removeItem(itemPos.x, itemPos.y, item);
		}

		std::cout << "Villager " << int(villager) << " equipped item " << int(item) << std::endl;
		state = JobState::Completed;
	}
}

