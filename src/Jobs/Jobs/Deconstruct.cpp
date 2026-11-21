#include "Deconstruct.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void Deconstruct::update() {
	if (item == entt::null) {
		state = JobState::Completed;
		return;
	}

	auto& v_pos = mainWorld.registry.get<Position>(villager);
	auto& pos = mainWorld.registry.get<Position>(item);

	auto closAdj = findClosestAdjTile(v_pos.x, v_pos.y, pos.x, pos.y);

	x = closAdj.first;
	y = closAdj.second;

	if (v_pos.x == x && v_pos.y == y) {
		clock += Clock::deltaTime;

		if (clock > 10.0f) {
			getTileRef(pos.x, pos.y).removeObject(pos.x, pos.y, item);

			state = JobState::Completed;
			return;
		}
	}
}