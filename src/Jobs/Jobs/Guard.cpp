#include "Jobs/Jobs/Guard.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"

void Guard::update() {
	auto& registry = mainWorld.registry;
	auto& guard = registry.get<OnGuard>(villager);
	auto& pos = registry.get<Position>(villager);

	if (guard.guardPost == entt::null) {
		auto post = findClosestItemType(pos.x, pos.y, 50, [&](entt::entity entity, entt::registry& reg, int, int) {
			auto* guardPost = reg.try_get<GuardPost>(entity);
			return guardPost && !guardPost->active && postToIgnore != entity;
			});

		if (post.has_value()) {
			postToIgnore = post->item;
			guard.guardPost = post->item;
			searchClock = 0.0f;
			clock = 0.0f;

			registry.get<GuardPost>(guard.guardPost).active = true;
		}
		else {
			return;
		}
	}

	searchClock += Clock::deltaTime;
	clock += Clock::deltaTime;

	if (searchClock > 30.0f) {
		if (registry.valid(guard.guardPost)) {
			registry.get<GuardPost>(guard.guardPost).active = false;
		}
		guard.guardPost = entt::null;
		return;
	}

	if (clock > 7.0f) {
		clock = 0.0f;

		auto& movable = registry.get<Movable>(villager);
		auto& gPos = registry.get<Position>(guard.guardPost);

		x = getRandomInt(gPos.x - 2, gPos.x + 2);
		y = getRandomInt(gPos.y - 2, gPos.y + 2);
	}
}
