#pragma once
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#include "Utility/mathUtils.h"
#include "World/Tile.h"

void updateSquadComponent();

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
};

struct SquadController {
    SquadState state = SquadState::IDLE;
    std::vector<entt::entity> members;

    std::pair<int, int> groupTargetPos = { 0, 0 };

    std::vector<Direction> macroFlowField;
    std::vector<SelectedTarget> targets;

    float idleWanderClock = 0.0f;
    float attackScanClock = 0.0f;
};