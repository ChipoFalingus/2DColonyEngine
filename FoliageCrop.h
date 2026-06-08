#pragma once

#include "Object.h"
#include "Tile.h"
#include "World.h"

class FoliageCrop : public Object {
public:
	float growTime;
	std::string produce;

	// Will need to write a global clock thing later
	sf::Clock growClock;

	int x, y;

	// Determines if dropped items will be added to ItemsToMove
	Colony* owner;

	FoliageCrop() : Object(), growTime(1.0f), produce("NULL"), x(0), y(0) {}

	void spawnProduce() {
		if (growClock.getElapsedTime().asSeconds() < growTime) {
			return;
		}

		growClock.restart();

		std::vector<std::pair<int, int>> neighbors = {
			{-1, 0}, {0, 1}, {0, -1}, {1, 0}
		};

		std::shuffle(neighbors.begin(), neighbors.end(), rng);

		for (auto& i : neighbors) {
			Tile& tile = getTileRef(x + i.first, y + i.second);
			if (tile.items.empty()) {
				auto item = ObjectRegistry::getInstance().get(produce);
				tile.addObject(item);
				if (owner) {
					mainWorld.addItemToMove(item, x + i.first, y + i.second);
				}
				return;
			}
		}
	}
};