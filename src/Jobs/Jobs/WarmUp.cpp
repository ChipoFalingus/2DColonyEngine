#include "WarmUp.h"
#include "World/World.h"

void WarmUp::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& tempNeed = mainWorld.registry.get<TemperatureNeed>(villager);
	auto& h_pos = mainWorld.registry.get<Position>(heat_source);

	if (!init) {

		if (auto* slots = mainWorld.registry.try_get<FirePitComponent>(heat_source)) {

			FirePitSlot* closestSlot = nullptr;
			int shortest = std::numeric_limits<int>::max();
			for (auto& slot : slots->slots) {
				if (slot.entity != entt::null) continue;

				int slotX = slot.x + h_pos.x;
				int slotY = slot.y + h_pos.y;

				int dx = std::abs(pos.x - slotX);
				int dy = std::abs(pos.y - slotY);

				if (dx + dy < shortest) {
					closestSlot = &slot;
					shortest = dx + dy;
				}
			}

			if (closestSlot) {
				closestSlot->entity = villager;
				x = closestSlot->x + h_pos.x;
				y = closestSlot->y + h_pos.y;
			}
			else {
				state = JobState::Completed;
				return;
			}
		}

		init = true;
	}

	clock += Clock::deltaTime;

	if (clock > 3.0f) {
		clock = 0.0f;
		priority--;

		if (priority <= 0) {
			if (auto* slots = mainWorld.registry.try_get<FirePitComponent>(heat_source)) {
				for (auto& slot : slots->slots) {
					if (slot.entity == villager) {
						slot.entity = entt::null;
						break;
					}
				}
			}
			state = JobState::Completed;
			return;
		}
	}
}

void WarmUp::onInterrupt() {
	if (auto* slots = mainWorld.registry.try_get<FirePitComponent>(heat_source)) {
		for (auto& slot : slots->slots) {
			if (slot.entity == villager) {
				slot.entity = entt::null;
				break;
			}
		}
	}
	state = JobState::Completed;
}