#include <random>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <queue>

#include "Villager.h"
#include "Tile.h"
#include "Pair.h"
#include "Item.h"
#include "Zombie.h"


std::vector<std::pair<int, int>> Villager::harvestTiles;
std::vector<Villager*> Villager::allVillagers;

std::vector<std::string> names = {
    //Male
    "Wyatt",
    "Caleb",
    "Dylan",
    "Ethan",
    "John",
    "Kingston",
    "Kyler",
    "Nick",
    "Noah",
    "Ryan",

    "James",
    "Michael",
    "Robert",
    "David",
	"William",
	"Richard",
	"Joseph",
	"Thomas",
	"Christopher",
	"Charles",
	"Daniel",
	"Matthew",
	"Anthony",
	"Mark",
	"Steven",
	"Donald",
	"Andrew",
	"Joshua",
	"Paul",
	"Kenneth",
	"Kevin",
	"Brian",
	"Timothy",
	"Ronald",
	"Jason",
	"George",
	"Edward",
	"Jeffrey",
    "Jacob",
	"Nicholas",
    "Gary",
	"Eric",
	"Jonathan",
	"Stephen",
	"Larry",
	"Justin",
	"Benjamin",
	"Scott",
	"Brandon",
	"Samuel",
	"Gregory",
	"Alexander",
	"Patrick",
	"Frank",
	"Jack",
	"Raymond",
	"Dennis",
	"Tyler",
	"Aaron",
    "Jerry",
	"Jimmy",

    //Female
    
	"Mary",
	"Patricia",
	"Jennifer",
	"Linda",
	"Elizabeth",
	"Barbara",
	"Susan",
	"Jessica",
	"Sarah",
	"Lisa",
	"Nancy",
	"Sandra",
	"Ashley",
	"Emily",
	"Kimberly",
	"Betty",
	"Margaret",
	"Donna",
	"Michelle",
	"Carol",
	"Amanda",
	"Melissa",
	"Deborah",
	"Stephanie",
	"Rebecca",
	"Sharon",
	"Laura",
	"Cynthia",
	"Amy",
	"Kathleen",
	"Angela",
	"Dorothy",
	"Shirley",
	"Emma",
	"Brenda",
	"Nicole",
	"Pamela",
	"Samantha",
	"Anna",
	"Katherine",
	"Christine",
	"Debra",
	"Rachel",
	"Olivia",
	"Carolyn",
	"Maria",
	"Janet",
	"Heather",
	"Diane",
	"Catherine",
	"Julie",

	//Fun names
	"There's really no limit to how long these names can be.",


};

std::vector<std::string> lastnames = {
    "Hoff",
    "Gordon",
    "Chiu",
    "Nahmias",
    "Bousquette",
    "Kim",
    "Hart",
    "Fart",
    "Lombardo",
    "Yim",
    "Krikorian",

	"Patel",
	"Garcia",
	"Smith",
	"Johnson",
	"Williams",
	"Brown",
	"Jones",
	"Miller",
	"Davis",
	"Rodriguez",
	"Martinez",
	"Hernandez",
	"Lopez",
	"Gonzalez",
	"Wilson",
	"Anderson",
	"Thomas",
	"Taylor",
	"Moore",
	"Jackson",
	"Martin",
	"Lee",
	"Perez",
	"Thompson",
	"White",
	"Harris",
	"Sanchez",
	"Clark",
	"Ramirez",
	"Lewis",
	"Neutron",
	"Coomer",

};

void Villager::depositItem(const Item& item, int amount) {



    currentPath = findPath(findClosestTileFurniture(chestFurniture));

    inventory.removeFromInventory(item, amount);
    chestFurniture.inventory.addToInventory(item, amount);


	std::cout << chestFurniture.inventory.slot[item] << " " << item.name << " in chest." << std::endl;

}


//void Villager::harvestTile(Item harvestingItem) {
//    if (isAtTile(target.first, target.second)) {
//        changeTileItem(target.first, target.second, std::make_unique<Item>(EMPTY_ITEM));
//        changeTileWalkable(target.first, target.second, true);
//
//        //inventory.slot.at(wheatSeed) += 2;
//    }
//}

std::pair<int, int> Villager::findClosestTileFurniture(const Furniture& item) {
    std::queue<std::pair<int, int>> frontier;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    frontier.push({ xPos, yPos });
    visited.insert({ xPos, yPos });

    while (!frontier.empty()) {
        auto current = frontier.front();
        frontier.pop();

        Tile& tile = getTileRef(current.first, current.second);
        if (tile.furnitureOnTile && tile.furnitureOnTile == &item) {
            for (auto& neighbor : getNeighbors(current.first, current.second)) {
                if (getTileRef(neighbor.first, neighbor.second).walkable) {
                    target = { current.first, current.second };
                    return neighbor;
                }
            }
        }

        for (auto& neighbor : getNeighbors(current.first, current.second)) {
            if (visited.count(neighbor) == 0) {
                visited.insert(neighbor);
                frontier.push(neighbor);
            }
        }
    }

    return { xPos, yPos };
}

void Villager::doWork() {

    if (health < lastHealth) {
        auto threat = findClosestCreatureType<Creature>(xPos, yPos, [this](Creature* c) { return c != this; });
		auto* retreatJob = new Retreat(this, JobType::None, threat);
		retreatJob->priority = 1000;
        jobQueue.push(retreatJob);

		auto function = [](Creature* c) {
			return dynamic_cast<Zombie*>(c) != nullptr;
			};

		jobQueue.push(new Attack(this, JobType::None, function));
        lastHealth = health;
    }

    if (jobQueue.empty()) {
		busy = false;
        return;
    }


    float elapsed = moveClock.getElapsedTime().asSeconds();
	currentJob = jobQueue.top();

    if (!currentPath.empty()) {
        if (elapsed > 1.0f / moveSpeed) {
            auto nextStep = currentPath.front();
            currentPath.erase(currentPath.begin());
            xPos = nextStep.first;
            yPos = nextStep.second;
            moveClock.restart();
        }
    }

    else if (currentJob) {
		
		
        currentJob->update();
        currentPath = findPath({ currentJob->x, currentJob->y });
        if (currentJob->completed) {
            jobQueue.pop();
            currentJob = nullptr;
        }
    }
}


//void Villager::getBestWeapon() {
//	int bestDamage = 0;
//
//    for (auto i : inventory.slot) {
//        if (i.first.name == "pistol") {
//			itemInHand = &i.first;
//        }
//    }
//}



