#include "Jobs/Jobs/AddToFurnace.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"
#include "Entities/Furnace.h"

void AddToFurnace::onAssignment(entt::entity villager) {

	auto& pos = mainWorld.registry.get<Position>(villager);

	FurnaceManager furnaceObject(mainWorld.registry);
	furnace = furnaceObject.findAndClaimFurnace(pos.x, pos.y);

	if (furnace == entt::null) {
		std::cout << "No Availabe Furnace Found" << std::endl;
		state = JobState::Completed;
		return;
	}

	staticRecipeEntity = ObjectRegistry::getInstance().getStaticObject(item);

	if (staticRecipeEntity == entt::null) {
		std::cout << "Could not find blueprint recipe for: " << item << std::endl;
		state = JobState::Completed;
		return;
	}

	auto& staticRegistry = ObjectRegistry::getInstance().getStaticRegistry();
	Craftable* c = staticRegistry.try_get<Craftable>(staticRecipeEntity);

	if (!c) {
		std::cout << "Craftable component not found for item " << item << ", cancelling job." << std::endl;
		state = JobState::Completed;
		return;
	}

	craftTime = c->craftTime;
	auto& ingredients = c->ingredients;
	reserve = findIngredientsForJob(ingredients);
	if (reserve.empty()) {
		std::cout << "No ingredients found for crafting job, cancelling job." << std::endl;
		state = JobState::Completed;
		return;
	}
}

void AddToFurnace::update() {
	auto pos = mainWorld.registry.try_get<Position>(villager);

	switch (jobState) {
	case (State::FetchingItems): {
		// Find the first ingredient in stockpile
		auto& itemObj = reserve[0];

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, itemObj.x, itemObj.y);
		x = adjLoc.first;
		y = adjLoc.second;

		if (pos->x == x && pos->y == y) {
			if (auto s = mainWorld.getColony().atStockpile(itemObj.x, itemObj.y)) {
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
		auto& staticRegistry = ObjectRegistry::getInstance().getStaticRegistry();
		auto* craftableComp = staticRegistry.try_get<Craftable>(staticRecipeEntity);
		if (!craftableComp) {
			state = JobState::Completed;
			return;
		}

		auto& furnacePos = mainWorld.registry.get<Position>(furnace);
		auto adjLoc = findClosestAdjTile(pos->x, pos->y, furnacePos.x, furnacePos.y);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos->x, pos->y, furnacePos.x, furnacePos.y)) {

			for (auto& i : mainWorld.objectManager.getObjectsAt(furnacePos.x, furnacePos.y)) {
				if (auto* f = mainWorld.registry.try_get<Furnace>(i)) {
					f->itemToCook = item;
					state = JobState::Completed;
					break;
				}
			}
		}
		break;
	}
	}

}

void AddToFurnace::onWaiting() {

	clock += Clock::deltaTime;

	if (clock > 1.0f) {
		clock = 0.0f;

		staticRecipeEntity = ObjectRegistry::getInstance().getStaticObject(item);
		Craftable* c = ObjectRegistry::getInstance().getStaticRegistry().try_get<Craftable>(staticRecipeEntity);

		auto pos = mainWorld.registry.try_get<Position>(villager);

		FurnaceManager furnaceObject(mainWorld.registry);
		furnace = furnaceObject.findAndClaimFurnace(pos->x, pos->y);

		if (furnace != entt::null) {

			reserve = findIngredientsForJob(c->ingredients);
			if (!reserve.empty()) {
				state = JobState::Queued;
				return;
			}
			else {
				mainWorld.registry.get<Claimable>(furnace).claimed = false;
				std::cout << "Ingredients Not Found" << std::endl;
			}
		}
	}
}