#include "DropEquipment.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void DropEquipment::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);

	std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, dropX, dropY);
	x = closestAdj.first;
	y = closestAdj.second;
	if (pos.x == x && pos.y == y) {

		auto* s = mainWorld.atStockpile(dropX, dropY);
		if (s) {
			mainWorld.registry.emplace_or_replace<Position>(item, dropX, dropY);

			auto name = mainWorld.registry.try_get<Name>(item);

			// add item to stockpile
			s->placeItem(item, dropX, dropY);
			mainWorld.registry.remove<NeedsMoving>(item);

			// add to object mananger
			mainWorld.objectManager.addObject(dropX, dropY, item);

			state = JobState::Completed;
		}
		else {
			auto name = mainWorld.registry.try_get<Name>(item);
			auto spotOpt = mainWorld.findStockpileSpotForItem(name->name, x, y);

			if (spotOpt) {
				auto& [stockpile, pos] = *spotOpt;
				stockpile->addItem(item, pos.first, pos.second);

				dropX = pos.first;
				dropY = pos.second;

				mainWorld.registry.emplace_or_replace<Position>(item, dropX, dropY);
				stockpile->addItem(item, dropX, dropY);
			}
			else {
				mainWorld.registry.emplace_or_replace<Position>(item, pos.x, pos.y);
				mainWorld.registry.remove<NeedsMoving>(item);
				getTileRef(pos.x, pos.y).addObject(pos.x, pos.y, name->name);

				state = JobState::Completed;
			}
		}
	}

}