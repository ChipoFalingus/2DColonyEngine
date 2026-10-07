#include "Sit.h"
#include "World/World.h"

void Sit::update() {

	if (!init) {
		mainWorld.registry.get<Claimable>(chair).claimed = true;

		auto& pos = mainWorld.registry.get<Position>(chair);
		x = pos.x;
		y = pos.y;

		init = true;
	}

	clock += Clock::deltaTime;
	auto& tiredNeed = mainWorld.registry.get<TiredNeed>(villager);

	if (clock > 2.0f) {
		tiredNeed.tiredness--;
		priority = tiredNeed.tiredness * 0.25f;
		clock = 0.0f;
	}

	if (tiredNeed.tiredness == 0) {
		mainWorld.registry.get<Claimable>(chair).claimed = false;
		state = JobState::Completed;
		return;
	}
}

void Sit::onInterrupt() {
	mainWorld.registry.get<Claimable>(chair).claimed = false;
	state = JobState::Completed;
}