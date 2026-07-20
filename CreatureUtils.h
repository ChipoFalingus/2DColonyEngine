#pragma once
#include "Creature.h"
#include "World.h"
#include "ItemLocation.h"
#include <functional>
#include <cmath>
#include <limits>
#include <optional>

//class Creature;
//class World;
//class Item;
//
//
// Finds certain item
std::optional<std::pair<int, int>> findClosestTileItem(const std::string item, int xPos, int yPos);

//bool isAtItem(const Object& item, int x, int y);
//
//std::pair<int, int> findBestTemperatureTile(int x, int y, int radius, float preferredTemp);
//
//template<typename T>
//Creature* findClosestCreatureType(int x, int y, int radius, std::function<bool(T*)> filter = {}) {
//    Creature* closest = nullptr;
//    float minDist = std::numeric_limits<float>::max();
//    float radiusSq = radius * radius;
//
//	std::vector<T*> potentialTargets;
//    for (auto& c : World::get().getAllCreatures()) {
//        T* typed = dynamic_cast<T*>(c.get());
//        if (typed) {
//            potentialTargets.push_back(typed);
//		}
//    }
//
//    for (auto& c : potentialTargets) {
//
//        T* typed = dynamic_cast<T*>(c);
//        if (!typed) continue;
//
//        if (filter && !filter(typed)) continue;
//
//        float dx = c->xPos - x;
//        float dy = c->yPos - y;
//        float dist = dx * dx + dy * dy;
//
//        if (dist > radiusSq) {
//            continue;
//        }
//
//        if (dist < minDist) {
//            minDist = dist;
//            closest = c;
//        }
//    }
//
//    return closest;
//}
//
//template<typename T>
//std::vector<T*> findAllCreaturesInRange(int x, int y, float radius, std::function<bool(T*)> filter = {}) {
//
//    float radiusSq = radius * radius;
//
//    std::vector<T*> result;
//    for (auto& c : World::get().getAllCreatures()) {
//        T* typed = dynamic_cast<T*>(c.get());
//        if (!typed) continue;
//
//        if (filter && !filter(typed)) continue;
//
//        float dx = c->xPos - x;
//        float dy = c->yPos - y;
//        float dist = dx * dx + dy * dy;
//
//        if (dist <= radiusSq) {
//            result.push_back(typed);
//        }
//    }
//
//    return result;
//}