#include "Nap.h"
#include "World/World.h"

void Nap::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& tiredNeed = mainWorld.registry.get<TiredNeed>(villager);

	if (!init) {
		auto* ownership = mainWorld.registry.try_get<Ownership>(villager);

		if (ownership && ownership->ownedBed != entt::null) {
			auto& bedPos = mainWorld.registry.get<Position>(ownership->ownedBed);
			x = bedPos.x;
			y = bedPos.y;
		}
		else {
			x = pos.x;
			y = pos.y;
		}

		init = true;
	}

	clock += Clock::deltaTime;

	if (pos.x == x && pos.y == y) {
		Tile& tile = getTileRef(pos.x, pos.y - 1);
		tile.setAnimType(Z);
	}

	if (clock > 2.0f) {
		tiredNeed.tiredness--;
		priority = tiredNeed.tiredness * 0.25f;
		clock = 0.0f;
	}

	if (tiredNeed.tiredness == 0) {
		Tile& tile = getTileRef(pos.x, pos.y - 1);
		tile.setAnimType(NONE);
		state = JobState::Completed;
		return;
	}
}

void Nap::onInterrupt() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	Tile& tile = getTileRef(pos.x, pos.y - 1);
	tile.setAnimType(NONE);

	state = JobState::Completed;
}