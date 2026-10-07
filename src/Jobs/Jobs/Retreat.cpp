#include "Jobs/Jobs/Retreat.h"
#include "World/World.h"

void Retreat::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& movable = mainWorld.registry.get<Movable>(villager);

	if (!init) {
		auto& jobComponent = mainWorld.registry.get<JobComponent>(villager);
		jobComponent.panicClock = 0.0f;
		//jobComponent.panicDuration = 10.0f;

		jobComponent.clearInterruptedJobs();

		init = true;
	}

	if (!foundPath) {
		int dim = 32;
		int half = dim / 2;
		std::vector<std::vector<float>> threatMap = buildThreatMap(pos.x, pos.y, dim);

		float bestScore = std::numeric_limits<float>::max();
		std::pair<int, int> bestLocation = { pos.x, pos.y };

		for (int i = -half; i < half; i++) {
			for (int j = -half; j < half; j++) {
				int worldX = pos.x + i;
				int worldY = pos.y + j;

				int localX = i + half;
				int localY = j + half;

				if (!getTileRef(worldX, worldY).walkable) {
					continue;
				}

				int dx = std::abs(worldX - pos.x);
				int dy = std::abs(worldY - pos.y);

				int distFromSelf = std::abs(i) + std::abs(j);

				float score = threatMap[localX][localY];
				score += distFromSelf * 0.05f;

				// rooms
				if (auto r = mainWorld.getRoomManager().getRoomAt(worldX, worldY)) {
					score -= 5.0f;
				}

				// lower is better
				if (score < bestScore) {
					bestScore = score;
					bestLocation = { worldX, worldY };
				}
			}
		}

		x = bestLocation.first;
		y = bestLocation.second;

		foundPath = true;
	}

	if (pos.x == x && pos.y == y) {
		foundPath = false;
	}

	auto closestThreat = findClosestItemType(pos.x, pos.y, 50, [&](entt::entity entity, entt::registry& reg, int x, int y) {
		return reg.try_get<Hostile>(entity) && (entity != villager);
		});

	if (!closestThreat.has_value()) {
		state = JobState::Completed;
	}
}