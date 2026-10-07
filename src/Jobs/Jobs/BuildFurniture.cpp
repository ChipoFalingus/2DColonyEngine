#include "Jobs/Jobs/BuildFurniture.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void BuildFurniture::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);

	switch (jobState)
	{
	case BuildFurniture::Getting: {

		auto closestAdj = findClosestAdjTile(pos.x, pos.y, fX, fY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(pos.x, pos.y, fX, fY)) {
			mainWorld.registry.remove<Position>(item);
			mainWorld.objectManager.removeItem(fX, fY, item);

			auto s = mainWorld.atStockpile(fX, fY);
			if (s) {
				s->removeItem(fX, fY, item);
			}

			jobState = BuildFurniture::Placing;
		}
		break;
	}

	case BuildFurniture::Placing: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (isAtTile(pos.x, pos.y, tX, tY)) {
			mainWorld.registry.emplace<Position>(item, tX, tY);
			mainWorld.registry.get<Claimable>(item).claimed = false;
			mainWorld.objectManager.addObject(tX, tY, item);
			removeBlueprint(tX, tY);
			state = JobState::Completed;

			break;

		}
	}
	default:
		break;
	}
}