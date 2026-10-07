#include "Jobs/Jobs/Plant.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void Plant::update() {

	Tile& tile = getTileRef(locX, locY);
	auto pos = mainWorld.registry.try_get<Position>(villager);

	switch (plantState)
	{
	case Plant::GetSeed: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, itemLocation.x, itemLocation.y);
		x = closestAdj.first;
		y = closestAdj.second;
		if (isAtTile(pos->x, pos->y, itemLocation.x, itemLocation.y)) {
			tile.removeObject(itemLocation.x, itemLocation.y, itemLocation.item);
			plantState = Plant::Move;
		}
		break;
	}
	case Plant::Move: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, locX, locY);

		x = closestAdj.first;
		y = closestAdj.second;

		if (pos->x == x && pos->y == y) {
			tile.changeTileType(tileType::SOIL);
			plantState = Plant::Tilling;
		}
		break;
	}
	case Plant::Tilling: {

		if (tile.type == tileType::SOIL) {
			plantState = Plant::Planting;
		}

		tillClock += Clock::deltaTime;

		int skill = mainWorld.registry.try_get<Skills>(villager)->getSkillLevel(type);
		if (tillClock > 20.0f * (10.0f / (skill + 3))) {
			tile.changeTileType(tileType::SOIL);
			plantState = Plant::Planting;
		}

		break;

	}
	case Plant::Planting: {
		plantClock += Clock::deltaTime;

		int skill = mainWorld.registry.try_get<Skills>(villager)->getSkillLevel(type);
		if (plantClock > 30.0f * (10.0f / (skill + 3))) {
			removeBlueprint(locX, locY);

			auto produce = ObjectRegistry::getInstance().getStaticComponent<Seed>(seed);
			tile.addObject(locX, locY, produce->produce);
			state = JobState::Completed;
		}

		break;
	}
	default:
		break;
	}
}