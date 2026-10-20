#include "World/World.h"
#include "Jobs/JobUtils.h"
#include "Entities/Movement.h"

void MovementManager::updateMovementSystem(float deltaTime) {
    auto view = registry.view<Position, Movable>();

    for (auto [entity, pos, movable] : view.each()) {

        movable.currentSpeed = movable.speed + calculateSpeedPenalty(entity);

        if (movable.dirX != 0 || movable.dirY != 0) {
            movable.hasTarget = false;
            movable.movementClock += deltaTime;

            if (movable.movementClock >= movable.currentSpeed) {
                movable.movementClock = 0.0f;
                int sepX = 0, sepY = 0;

                forEachInRange(pos.x, pos.y, 3, [&](entt::entity otherEntity, entt::registry& reg, int x, int y) {
                    if (otherEntity == entity) return;
                    if (!reg.try_get<SquadMemberComponent>(otherEntity)) return;
                    auto& otherPos = reg.get<Position>(otherEntity);

                    int dx = pos.x - otherPos.x;
                    int dy = pos.y - otherPos.y;

                    if (dx * dx + dy * dy <= 3) {
                        sepX += dx;
                        sepY += dy;
                    }

                    });

                int pushX = (sepX > 0) - (sepX < 0);
                int pushY = (sepY > 0) - (sepY < 0);

                int stepX = std::clamp(movable.dirX + pushX, -1, 1);
                int stepY = std::clamp(movable.dirY + pushY, -1, 1);

                int newX = pos.x + stepX;
                int newY = pos.y + stepY;

                if (getTileRef(newX, newY).walkable) {
                    manager.removeItem(pos.x, pos.y, entity);
                    manager.addObject(newX, newY, entity);
                    pos.x = newX;
                    pos.y = newY;
                }

                movable.dirX = 0;
                movable.dirY = 0;
                continue;
            }
        }

        if (!movable.hasTarget) continue;
        movable.movementClock += deltaTime;

        if (movable.path.empty()) {
            movable.path = findPath(pos.x, pos.y, { movable.targetX, movable.targetY });

            if (movable.path.empty()) {
                movable.hasTarget = false;
                continue;
            }
        }

        if (movable.movementClock >= movable.currentSpeed) {
            movable.movementClock = 0.0f;

            if (!movable.path.empty()) {
                int newX = movable.path[0].first;
                int newY = movable.path[0].second;

                if (!getTileRef(newX, newY).walkable) {
                    movable.path = findPath(pos.x, pos.y, { movable.targetX, movable.targetY });
                    newX = movable.path[0].first;
                    newY = movable.path[0].second;
                }

                manager.removeItem(pos.x, pos.y, entity);
                manager.addObject(newX, newY, entity);

                pos.x = newX;
                pos.y = newY;

                movable.path.erase(movable.path.begin());
            }

            if (pos.x == movable.targetX && pos.y == movable.targetY) {
                movable.hasTarget = false;
                movable.path.clear();
            }
        }
    }
}
