#pragma once
#include <string>
#include <entt/entt.hpp>

class MovementManager {
private:
    entt::registry& registry;
    ObjectManager& manager;
public:
    MovementManager(entt::registry& reg, ObjectManager& manager) : registry(reg), manager(manager) {}

    void updateMovementSystem(float deltaTime);
};