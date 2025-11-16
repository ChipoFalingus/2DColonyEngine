#include "Creature.h"
#include "CreatureUtils.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "Tile.h"
#include "Pair.h"
#include "Light.h"


#include <typeindex>



std::vector<std::pair<int, int>> Creature::findPath(std::pair<int, int> goal) {
    using namespace std;

    if (!getTileRef(goal.first, goal.second).walkable) {
        return {};
    }

    std::pair<int, int> start = { xPos, yPos };

    priority_queue<
        pair<int, pair<int, int>>,
        vector<pair<int, pair<int, int>>>,
        greater<>
    > openSet;

    unordered_map<pair<int, int>, int, pair_hash> gScore;
    unordered_map<pair<int, int>, int, pair_hash> fScore;
    unordered_map<pair<int, int>, pair<int, int>, pair_hash> cameFrom;

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push({ fScore[start], start });

    vector<pair<int, int>> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        { 1, 1 }, {-1, 1}, {1, -1}, {-1, -1}
    };

    while (!openSet.empty()) {
        auto current = openSet.top().second;
        openSet.pop();

        if (current == goal) {
            vector<pair<int, int>> path;
            while (cameFrom.find(current) != cameFrom.end()) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        for (auto dir : directions) {
            int neighborX = current.first + dir.first;
            int neighborY = current.second + dir.second;
            pair<int, int> neighbor = { neighborX, neighborY };

            Tile& neighborTile = getTileRef(neighborX, neighborY);
            if (!neighborTile.walkable) continue;

            int tentativeG = gScore[current] + 1;

            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeG;
                fScore[neighbor] = tentativeG + heuristic(neighbor, goal);
                openSet.push({ fScore[neighbor], neighbor });
            }
        }
    }

    return {};
}


bool Creature::isAtTile(int x, int y) {
    if (x == xPos && y == yPos) {
        return true;
    }
    for (auto& i : getNeighbors(xPos, yPos)) {
        if (x == i.first && y == i.second) {
            return true;
        }
    }
    return false;
}


std::vector<std::pair<int, int>> Creature::getNeighbors(int x, int y) {
    return { {x - 1, y}, {x, y - 1}, {x + 1, y}, {x, y + 1} };
}


int Creature::heuristic(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

float Creature::getDistance(Creature* other) {
    return sqrt(pow(other->xPos - xPos, 2) + pow(other->yPos - yPos, 2));
}

void Creature::attack() {
    if (!targetCreature) return; 
    if (targetCreature->dead) {
        targetCreature = nullptr;
        currentPath.clear();
        //itemInHand = &EMPTY_ITEM;
		return;
    }

    float dx = targetCreature->xPos - xPos;
    float dy = targetCreature->yPos - yPos;
    float distance = std::sqrt(dx * dx + dy * dy);

    float range = (!itemInHand) ? 1.0f : itemInHand->getRange();

    if (distance <= range) {
        
		currentPath.clear(); // Stop moving when in range

        if (!itemInHand) {
            if (attackClock.getElapsedTime().asSeconds() > 1.0f) {
                std::cout << "Attacking for 1 damage." << std::endl;
                targetCreature->health -= 1;
				attackClock.restart();
            }
        }
        else {
            if (attackClock.getElapsedTime().asSeconds() > itemInHand->getAttackCooldown()) {


                LightManager::addLight(glm::vec2(xPixels, yPixels), glm::vec3(1.0f, 1.0f, 0.0f), 300.0f, 1.5f, 0.02f);
                //LightManager::addLight(glm::vec2(960, 540), glm::vec3(1.0f, 0.5f, 0.0f), 200.0f, 2.5f, 0.02f);

                std::cout << "Attacking with " << itemInHand->name << " for " << itemInHand->getDamage() << " damage." << std::endl;
                int dmg = itemInHand->getDamage();
                targetCreature->health -= dmg;
				attackClock.restart();
            }
			
            if (targetCreature->health < 1) {
				std::cout << "Target died." << std::endl;
                targetCreature->dead = true;
                targetCreature = nullptr;
                currentPath.clear();
                //itemInHand = nullptr;
            }
        }

    }
}

void Creature::stop() {
    currentPath.clear();
}   

void Creature::doWork() {};
