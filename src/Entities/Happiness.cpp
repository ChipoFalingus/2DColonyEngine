#include "Happiness.h"
#include "Entities/ItemComponents.h"
#include "Entities/CreatureComponents.h"
#include "World/World.h"

void addThought(Thought thought, std::vector<Thought>& list) {
	for (auto& t : list) {
		if (t.id == thought.id) {
			t.duration = thought.duration;
			return;
		};
	}
	std::cout << thought.thought << std::endl;
	list.push_back(thought);
}

void updateHappiness(entt::registry& registry, float dt) {
	auto view = registry.view<Position, Happiness>();
	for (auto [entity, pos, happiness] : view.each()) {

		// Update current thoughts
		auto& thoughts = happiness.active_thoughts;
		float total_modifier = 0.0f;
		for (auto it = thoughts.begin(); it != thoughts.end(); ) {
			it->duration -= dt;
			if (it->duration <= 0.0f) {
				it = thoughts.erase(it);
			}
			else {
				total_modifier += it->modifier;
				++it;
			}
		}

		happiness.clock += dt;
		if (happiness.clock >= 2.0f) {
			happiness.clock = 0.0f;

			auto& pos = registry.get<Position>(entity);
			auto& name = registry.get<Name>(entity);

			if (mainWorld.getLightManager().getLightMapIndex(pos.x, pos.y) < 0.4f) {
				addThought({ "dark", name.name + " is scared of the dark", -10, 5.0f }, thoughts);
			}

			if (auto* h = registry.try_get<HungerNeed>(entity)) {
				if (h->hunger > 90) {
					addThought({ "hungry", name.name + " is starving", -25, 5.0f }, thoughts);
				}
				else if (h->hunger > 70) {
					addThought({ "hungry", name.name + " is really hungry", -15, 5.0f }, thoughts);
				}
				else if (h->hunger > 50) {
					addThought({ "hungry", name.name + " is hungry", -5, 5.0f }, thoughts);
				}
			}

			if (auto* s = registry.try_get<Social>(entity)) {
				auto& name = registry.get<Name>(entity);
				if (s->social < 30) {
					addThought({ "lonely", name.name + " is lonely", -15, 1.0f }, thoughts);
				}
			}
		}

		happiness.current_happiness = happiness.base_happiness + total_modifier;

	}
}