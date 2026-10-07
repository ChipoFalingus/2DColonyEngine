#include "Idle.h"
#include "World/World.h"

void Idle::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& movable = mainWorld.registry.get<Movable>(villager);

	int range = 10;

	if (!initialized) {

		int nx = getRandomInt(pos.x - range, pos.x + range);
		int ny = getRandomInt(pos.y - range, pos.y + range);

		if (!getTileRef(nx, ny).walkable) return;

		x = nx;
		y = ny;

		waitTime = getRandomFloat(1.0f, 10.0f);
		clock = 0.0f;

		initialized = true;
	}

	if (pos.x == x && pos.y == y) {
		clock += Clock::deltaTime;

		if (clock > waitTime) {
			movable.currentSpeed = movable.speed;
			state = JobState::Completed;
		}
	}
}

void Idle::onInterrupt() {
	state = JobState::Completed;
}
