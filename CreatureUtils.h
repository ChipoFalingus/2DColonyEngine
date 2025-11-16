#pragma once
#include "Creature.h"
#include <functional>

class Creature;
class Item;

std::vector<std::pair<int, int>> getNeighbors(int x, int y);

std::pair<int, int> findClosestTileItem(Item item, int xPos, int yPos);

bool isAtItem(Item item, int x, int y);

template<typename T>
Creature* findClosestCreatureType(int x, int y, std::function<bool(T*)> filter = nullptr) {
    Creature* closest = nullptr;
    float minDist = std::numeric_limits<float>::max();

    for (auto* c : Creature::allCreatures) {
        if (filter && !filter(dynamic_cast<T*>(c))) continue;
        if (dynamic_cast<T*>(c) != nullptr) {
            float dist = pow(c->xPos - x, 2) + pow(c->yPos - y, 2);
            if (dist < minDist) {
                minDist = dist;
                closest = c;
            }
        }
    }

    return closest;
}