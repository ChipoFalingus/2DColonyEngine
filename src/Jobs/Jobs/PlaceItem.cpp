#include "PlaceItem.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void PlaceItem::update() {
	Tile& tile = getTileRef(locX, locY);

	const int vX = mainWorld.registry.get<Position>(villager).x;
	const int vY = mainWorld.registry.get<Position>(villager).y;

	std::pair<int, int> closestAdj = findClosestAdjTile(vX, vY, x, y);

	x = closestAdj.first;
	y = closestAdj.second;

	if (vX == x && vY == y) {
		const std::string itemName = mainWorld.registry.get<Name>(itemToPlace).name;
		tile.addObject(locX, locY, itemName);
		state = JobState::Completed;
	}
}