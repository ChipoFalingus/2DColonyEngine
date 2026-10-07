#include "Sleep.h"
#include "World/World.h"

void Sleep::update() {
	mainWorld.registry.get<JobComponent>(villager).activity_state = ActivityState::Sleeping;
	auto& tiredComponent = mainWorld.registry.get<TiredNeed>(villager);
	auto* ownership = mainWorld.registry.try_get<Ownership>(villager);

	auto& pos = mainWorld.registry.get<Position>(villager);
	if (ownership && ownership->ownedBed != entt::null) {
		auto& bedPos = mainWorld.registry.get<Position>(ownership->ownedBed);
		x = bedPos.x;
		y = bedPos.y;
	}
	else {
		x = pos.x;
		y = pos.y;
	}

	Tile& tile = getTileRef(x, y - 1);

	if (pos.x == x && pos.y == y && !sleeping) {
		tile.setAnimType(Z);

		sleeping = true;

	}
	else if (sleeping && mainWorld.dayCycle.getTimePeriod() == TimePeriod::Morning) {
		tile.setAnimType(NONE);

		sleeping = false;
		tiredComponent.tiredness = 0;
		state = JobState::Completed;
	}
}