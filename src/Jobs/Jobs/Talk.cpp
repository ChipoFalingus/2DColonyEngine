#include "Talk.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void Talk::update() {

	if (!mainWorld.registry.valid(other)) {
		state = JobState::Completed;
		return;
	}

	auto& jobComponent = mainWorld.registry.get<JobComponent>(villager);
	jobComponent.activity_state = ActivityState::Socializing;

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& otherPos = mainWorld.registry.get<Position>(other);

	switch (talkState) {
	case (State::WalkTo): {
		int dx = std::abs(pos.x - otherPos.x);
		int dy = std::abs(pos.y - otherPos.y);

		if (dx + dy <= 2) {
			x = pos.x;
			y = pos.y;

			talkState = State::TalkTo;
		}
		else {
			auto adj = findClosestAdjTile(pos.x, pos.y, otherPos.x, otherPos.y);
			x = adj.first;
			y = adj.second;
		}

		break;
	}
	case (State::TalkTo): {
		Tile& tile = getTileRef(pos.x, pos.y - 1);
		Tile& otherTile = getTileRef(otherPos.x, otherPos.y - 1);
		tile.setAnimType(SPEECH_BUBBLE);

		clock += Clock::deltaTime;

		auto& socialNeed = mainWorld.registry.get<Social>(villager);
		auto& otherSocialNeed = mainWorld.registry.get<Social>(other);

		if (clock > 10.0f) {
			socialNeed.social = 100;
			otherSocialNeed.social = 100;

			otherTile.setAnimType(NONE);
			tile.setAnimType(NONE);

			state = JobState::Completed;
		}

		break;
	}
	}
}