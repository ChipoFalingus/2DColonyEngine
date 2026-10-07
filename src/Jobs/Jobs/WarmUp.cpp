#include "WarmUp.h"
#include "World/World.h"

void WarmUp::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& tempNeed = mainWorld.registry.get<TemperatureNeed>(villager);

	if (!init) {
		location = findClosestItemType(pos.x, pos.y, 50, [&](entt::entity entity, entt::registry& reg, int x, int y) {
			auto* temp = reg.try_get<HeatEmitter>(entity);
			return temp;
			});

		if (location.has_value()) {

			if (auto* slots = mainWorld.registry.try_get<FirePitComponent>(location->item)) {
				FirePitSlot* closestSlot = nullptr;
				int shortest = std::numeric_limits<int>::max();
				for (auto& slot : slots->slots) {
					if (slot.entity != entt::null) continue;

					int slotX = slot.x + location->x;
					int slotY = slot.y + location->y;

					int dx = std::abs(pos.x - slotX);
					int dy = std::abs(pos.y - slotY);

					if (dx + dy < shortest) {
						closestSlot = &slot;
						shortest = dx + dy;
					}
				}

				if (closestSlot) {
					closestSlot->entity = villager;
					x = closestSlot->x + location->x;
					y = closestSlot->y + location->y;

					std::cout << "Moving to " << x << ", " << y << std::endl;
				}
				else {
					std::cout << "No available slots at heat source, staying in place" << std::endl;
					state = JobState::Completed;
					return;
				}
			}
		}
		else {
			//std::cout << "No heat source found, staying in place" << std::endl;
			state = JobState::Completed;
			return;
		}

		init = true;
	}

	clock += Clock::deltaTime;

	if (clock > 30.0f) {
		/*priority--;

		if (priority <= 0) {
			state = JobState::Completed;
			return;
		}*/
		if (auto* slots = mainWorld.registry.try_get<FirePitComponent>(location->item)) {
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

void WarmUp::onInterrupt() {
	if (auto* slots = mainWorld.registry.try_get<FirePitComponent>(location->item)) {
		for (auto& slot : slots->slots) {
			if (slot.entity == villager) {
				slot.entity = entt::null;
				break;
			}
		}
	}
	state = JobState::Completed;
}