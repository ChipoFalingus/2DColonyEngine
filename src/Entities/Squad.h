#pragma once
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>
#include <entt/entt.hpp>

#include "Utility/mathUtils.h"
#include "World/Tile.h"
#include "Utility/ItemUtils.h"
#include "Utility/Clock.h"
#include "Entities/CreatureComponents.h"

enum SquadState {
    IDLE,
    MOVING,
    ATTACKING
};

struct SelectedTarget {
    entt::entity target;
    int score;

    int target_population;
};

struct SquadMemberComponent {
    entt::entity squadEntity = entt::null;
    SelectedTarget target;
    float attackClock = 0.0f;
};

struct SquadController {
    SquadState state = SquadState::IDLE;
    std::vector<entt::entity> members;

    std::pair<int, int> groupTargetPos = { 0, 0 };
	std::pair<int, int> centroid;

    std::vector<Direction> macroFlowField;
    std::vector<SelectedTarget> targets;

    float idleWanderClock = 0.0f;
    float attackScanClock = 0.0f;
};

class SquadManager {
private:

	float mergeClock = 0.0f;

	void updateSquadMovement(entt::entity squad, entt::registry& registry);
	void onIdle(entt::entity squad, entt::registry& registry);
	void onAttack(entt::entity squad, entt::registry& registry);
	void findTargets(entt::entity entity, entt::registry& registry);
	void updateAveragePosition(entt::entity entity, entt::registry& registry);

	void merge(entt::entity squad1, entt::entity squad2, entt::registry& registry);
	void split(entt::entity entity, entt::registry& registry);

	SelectedTarget chooseTarget(entt::entity squad, entt::entity member, entt::registry& registry);

public:
	void updateSquadComponent(entt::registry& registry) {
		auto view = registry.view<SquadController>();
		std::vector<entt::entity> empty_squads;

		for (auto& i : view) {
			findTargets(i, registry);
			updateAveragePosition(i, registry);
		}
		
		mergeClock += Clock::deltaTime;

		const int radius = 20 * 20;

		if (mergeClock > 1.0f) {
			mergeClock = 0.0f;
			std::vector<entt::entity> squads(view.begin(), view.end());

			for (size_t i = 0; i < squads.size(); i++) {
				for (size_t j = i + 1; j < squads.size(); j++) {

					if (!registry.valid(squads[j]) || !registry.valid(squads[i])) continue;

					auto& squad2 = registry.get<SquadController>(squads[j]);
					auto& squad1 = registry.get<SquadController>(squads[i]);

					int dx = squad1.centroid.first - squad2.centroid.first;
					int dy = squad1.centroid.second - squad2.centroid.second;

					if (dx * dx + dy * dy < radius) {
						merge(squads[i], squads[j], registry);
					}
				}
			}
		}

		for (auto& i : view) {
			updateSquadMovement(i, registry);
			auto& controller = registry.get<SquadController>(i);
			if (controller.members.empty()) {
				empty_squads.push_back(i);
			}

			std::erase_if(controller.members, [&registry](entt::entity e) {
				auto i = registry.try_get<Health>(e);
				return !i || i->health <= 0;
				});
		}

		for (auto& i : empty_squads) {
			registry.destroy(i);
		}
	}
};