#include "Jobs/Jobs/FindFood.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void FindFood::update() {

	mainWorld.registry.get<Claimable>(food).claimed = true;
	mainWorld.registry.get<JobComponent>(villager).activity_state = ActivityState::Eating;

	auto& pos = mainWorld.registry.get<Position>(villager);

	switch (foodState) {

	case State::Grab: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(pos.x, pos.y, tX, tY)) {
			mainWorld.registry.remove<Position>(food);
			mainWorld.objectManager.removeItem(tX, tY, food);

			auto s = mainWorld.atStockpile(tX, tY);
			if (s) {
				s->removeItem(tX, tY, food);
			}
			foodState = State::Find;
		}
		break;
	}


	case State::Find: {
		place = findClosestItemType(pos.x, pos.y, 25, [&](entt::entity item, entt::registry& registry, int x, int y) {
			if (!registry.any_of<Sittable>(item)) return false;
			if (registry.get<Claimable>(item).claimed) return false;

			static const std::pair<int, int> dirs[4] = {
				{0,1},{0,-1},{1,0},{-1,0}
			};

			for (auto& d : dirs) {
				int nx = x + d.first;
				int ny = y + d.second;

				const Tile& t = getTileRef(nx, ny);

				for (auto& item : mainWorld.objectManager.getObjectsAt(nx, ny)) {
					if (registry.any_of<Table>(item)) {
						return true;
					}
				}
			}

			return false;

			});
		if (place.has_value()) {
			auto& pos = mainWorld.registry.get<Position>(place->item);
			std::cout << "Found Table + Chair at " << pos.x << " " << pos.y << std::endl;
			mainWorld.registry.get<Claimable>(place->item).claimed = true;
		}
		foodState = State::Eat;
		break;
	}

	case State::Eat: {
		if (place.has_value()) {
			x = place->x;
			y = place->y;
		}
		else {
			x = pos.x;
			y = pos.y;
		}

		if (pos.x == x && pos.y == y) {
			eatTimer += Clock::deltaTime;

			if (eatTimer > 10.0f) {

				if (auto i = mainWorld.registry.try_get<Nutritional>(food)) {
					auto& hunger = mainWorld.registry.get<HungerNeed>(villager).hunger;
					hunger += i->nutrition;
				}

				if (place.has_value()) {
					mainWorld.registry.get<Claimable>(place->item).claimed = false;
				}
				state = JobState::Completed;
			}
		}

		break;
	}
	}
}