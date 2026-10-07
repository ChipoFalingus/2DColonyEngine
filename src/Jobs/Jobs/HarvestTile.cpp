#include "HarvestTile.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void HarvestTile::update() {
	Tile& tile = getTileRef(locX, locY);

	auto itemPos = mainWorld.registry.try_get<Position>(item);
	locX = itemPos->x;
	locY = itemPos->y;

	switch (harvestState)
	{
	case HarvestTile::MovingToTile: {
		auto pos = mainWorld.registry.try_get<Position>(villager);
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, locX, locY);

		x = closestAdj.first;
		y = closestAdj.second;

		bool adjacent = isAtTile(pos->x, pos->y, locX, locY);

		if (adjacent) {
			harvestState = HarvestTile::Harvesting;
		}

		break;
	}
	case HarvestTile::Harvesting: {
		harvestClock += Clock::deltaTime;

		int skill = mainWorld.registry.try_get<Skills>(villager)->getSkillLevel(type);
		if (harvestClock > 1.0f * (10.0f / skill)) {

			auto drop = mainWorld.registry.try_get<Harvestable>(item);

			for (auto& i : drop->drops) {
				if (getRandomFloat(0.0f, 1.0f) < i.odds) {
					tile.addObject(locX, locY, i.item, true);
				}
			}

			tile.removeObject(locX, locY, item);
			tile.anim.type = animType::NONE;

			tile.markedForHarvest = false;
			state = JobState::Completed;
		}
	}
								break;
	default:
		break;
	}

}