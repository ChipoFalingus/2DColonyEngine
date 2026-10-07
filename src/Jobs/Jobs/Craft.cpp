#include "Jobs/Jobs/Craft.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void Craft::update() {
	auto pos = mainWorld.registry.try_get<Position>(villager);
	auto& staticRegistry = ObjectRegistry::getInstance().getStaticRegistry();

	if (!init) {
		staticRecipeEntity = ObjectRegistry::getInstance().getStaticObject(item);

		if (staticRecipeEntity == entt::null) {
			std::cout << "Could not find blueprint recipe for: " << item << std::endl;
			state = JobState::Completed;
			return;
		}

		Craftable* c = staticRegistry.try_get<Craftable>(staticRecipeEntity);

		if (!c) {
			std::cout << "Craftable component not found for item " << item << ", cancelling job." << std::endl;
			state = JobState::Completed;
			return;
		}

		craftTime = c->craftTime;
		auto& ingredients = c->ingredients;
		reserve = findIngredientsForJob(ingredients);
		init = true;
		if (reserve.empty()) {
			std::cout << "No ingredients found for crafting job, cancelling job." << std::endl;
			state = JobState::Completed;
			return;
		}
	}

	switch (jobState) {
	case (State::FetchingItems): {
		// Find the first ingredient in stockpile
		auto& itemObj = reserve[0];

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, itemObj.x, itemObj.y);
		x = adjLoc.first;
		y = adjLoc.second;

		if (pos->x == x && pos->y == y) {
			if (auto s = mainWorld.atStockpile(itemObj.x, itemObj.y)) {
				getTileRef(itemObj.x, itemObj.y).removeObject(itemObj.x, itemObj.y, itemObj.item);
			}

			reserve.erase(reserve.begin());

			if (reserve.empty()) {
				jobState = State::Crafting;
			}
		}
		break;
	}
	case (State::Crafting): {

		auto* craftableComp = staticRegistry.try_get<Craftable>(staticRecipeEntity);
		if (!craftableComp) {
			state = JobState::Completed;
			return;
		}

		auto loc = findClosestTileItem(craftableComp->benchRequired, pos->x, pos->y);

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, loc->first, loc->second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos->x, pos->y, loc->first, loc->second)) {

			clock += Clock::deltaTime;

			if (clock >= craftTime) {
				for (int i = 0; i < craftableComp->quantity; i++) {
					getTileRef(loc->first, loc->second).addObject(loc->first, loc->second, item, true);
				}
				state = JobState::Completed;
			}

		}
		break;
	}
	}
}