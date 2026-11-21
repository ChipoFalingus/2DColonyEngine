#pragma once
#include <string>
#include <entt/entt.hpp>

enum WalkStatus {
	Idling,
	Working,
	Running
};

struct Movable {
	float speed = 0.8f;
	float currentSpeed = 0.2f;

	float movementClock = 0.0f;

	int targetX = 0;
	int targetY = 0;
	bool hasTarget = false;

	std::vector<std::pair<int, int>> path;

	WalkStatus walk_status = WalkStatus::Idling;

	// Squad movement
	int dirX = 0;
	int dirY = 0;

	bool ignoreSeparation = false;

	void newPath(int newX, int newY) {
		path.clear();
		targetX = newX;
		targetY = newY;
	}
};

class MovementManager {
private:
    entt::registry& registry;
    ObjectManager& manager;
public:
    MovementManager(entt::registry& reg, ObjectManager& manager) : registry(reg), manager(manager) {}

    void updateMovementSystem(float deltaTime);
};