#include "Creature.h"
#include "CreatureUtils.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "Tile.h"
#include "Pair.h"
#include "Light.h"

#include <typeindex>

float Creature::getDistance(Creature* other) {
    int x = other->xPos - this->xPos;
    int y = other->yPos - this->yPos;
    return sqrt(x * x + y * y);
}

void Creature::doWork() {};

void Creature::takeDamage(int dmg, Creature* attacker) {
    if (dead) return;

    health -= dmg;

    if (!targetCreature && attacker && !attacker->dead) {
        targetCreature = attacker;
    }

    if (health <= 0) {
        dead = true;
    }
}
