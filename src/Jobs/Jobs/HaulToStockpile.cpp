#include "Jobs/Jobs/HaulToStockpile.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void HaulToStockpile::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);

	switch (moveState) {


	case (State::PickUpItem): {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, fromX, fromY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (pos.x == x && pos.y == y) {

			mainWorld.objectManager.removeItem(fromX, fromY, itemToMove);

			if (mainWorld.registry.any_of<Position>(itemToMove)) {
				mainWorld.registry.remove<Position>(itemToMove);
			}

			moveState = State::Move;
		}
		break;

	}

	case (State::Move): {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, toX, toY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (pos.x == x && pos.y == y) {

			auto* s = mainWorld.atStockpile(toX, toY);
			if (s) {
				mainWorld.registry.emplace_or_replace<Position>(itemToMove, toX, toY);

				auto name = mainWorld.registry.try_get<Name>(itemToMove);

				// add item to stockpile
				s->placeItem(itemToMove, toX, toY);

				// add to object mananger
				mainWorld.objectManager.addObject(toX, toY, itemToMove);

				state = JobState::Completed;
			}
			else {
				auto name = mainWorld.registry.try_get<Name>(itemToMove);
				auto spotOpt = mainWorld.findStockpileSpotForItem(name->name, x, y);

				if (spotOpt) {
					auto& [stockpile, pos] = *spotOpt;
					stockpile->addItem(itemToMove, pos.first, pos.second);

					toX = pos.first;
					toY = pos.second;

					mainWorld.registry.emplace_or_replace<Position>(itemToMove, toX, toY);
					stockpile->addItem(itemToMove, toX, toY);
				}
				else {
					mainWorld.registry.emplace_or_replace<Position>(itemToMove, pos.x, pos.y);
					mainWorld.registry.remove<NeedsMoving>(itemToMove);
					getTileRef(pos.x, pos.y).addObject(pos.x, pos.y, name->name);

					state = JobState::Completed;
				}
			}
		}
		break;
	}

	case (State::Drop):

		break;
	}
}
