#include "Build.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

Build::Build(entt::entity v, entt::entity tool, SkillType skillType, std::string itemName, int locX, int locY)
	: Job(v, tool, skillType), itemName(itemName), locX(locX), locY(locY)
{
	staticRecipeEntity = ObjectRegistry::getInstance().getStaticObject(itemName);

	Craftable* c = ObjectRegistry::getInstance().getStaticRegistry().try_get<Craftable>(staticRecipeEntity);

	if (!c) {
		std::cout << "Item not craftable" << std::endl;
		state = JobState::Completed;
		return;
	}

	craftTime = c->craftTime;
	reserve = findIngredientsForJob(c->ingredients);
	if (reserve.empty()) {
		std::cout << "Ingredients for building not found" << std::endl;
		state = JobState::Waiting;
		return;
	}
}

void Build::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);

	if (!grabbedAllItems) {
		auto& itemObj = reserve.front();

		int xPos = itemObj.x;
		int yPos = itemObj.y;

		auto adjLoc = findClosestAdjTile(pos.x, pos.y, xPos, yPos);
		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos.x, pos.y, xPos, yPos)) {
			getTileRef(xPos, yPos).removeObject(xPos, yPos, itemObj.item);
			reserve.erase(reserve.begin());

			if (reserve.empty()) {
				grabbedAllItems = true;
			}
		}
	}
	else {
		std::pair<int, int> loc = { locX, locY };

		auto adjLoc = findClosestAdjTile(pos.x, pos.y, loc.first, loc.second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos.x, pos.y, loc.first, loc.second)) {

			clock += Clock::deltaTime;

			if (clock < craftTime) return;

			removeBlueprint(loc.first, loc.second);

			auto item = ObjectRegistry::getInstance().createInstance(itemName, mainWorld.registry);
			mainWorld.registry.emplace<Desconstructable>(item);

			mainWorld.registry.emplace<Position>(item, loc.first, loc.second);

			if (mainWorld.registry.try_get<Structure>(item)) {
				getTileRef(loc.first, loc.second).walkable = false;
			}
			mainWorld.objectManager.addObject(loc.first, loc.second, item);

			mainWorld.getColony().getRoomManager().findRoom(loc.first, loc.second);

			const std::pair<int, int> dirs[4] = {
				{0, 1}, {1, 0}, {-1, 0}, {0, -1}
			};

			for (auto& dir : dirs) {
				int nx = loc.first + dir.first;
				int ny = loc.second + dir.second;

				// should be generic floor tag
				if (mainWorld.objectManager.has(nx, ny, "Stone Floor")) {
					mainWorld.getColony().getRoomManager().findRoom(nx, ny);
				}
			}

			mainWorld.getColony().updateBorders(loc.first, loc.second);
			state = JobState::Completed;
		}
	}
}

void Build::onWaiting() {
	clock += Clock::deltaTime;

	if (clock > 1.0f) {
		clock = 0.0f;

		Craftable* c = ObjectRegistry::getInstance().getStaticRegistry().try_get<Craftable>(staticRecipeEntity);
		reserve = findIngredientsForJob(c->ingredients);

		if (!reserve.empty()) {
			state = JobState::Queued;
		}
	}
}