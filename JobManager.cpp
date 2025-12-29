#include "Job.h"
#include "Item.h"
#include "Light.h"
#include "CreatureUtils.h"
#include "Villager.h"
#include "Globals.h"
#include "HarvestRules.h"

std::vector<Job*> JobManager::JobList;
bool isAtTile(int xPos, int yPos, int xLoc, int yLoc);
std::pair<int, int> findClosestAdjTile(int xPos, int yPos, int xTile, int yTile);

void JobManager::findBestColonistForJob(Job& job) {
	// Needs to handle multiple villagers having the same job
	Villager* bestVillager = nullptr;
	float lowestDistance = INFINITY;



	for (auto* i : Villager::allVillagers) {
		if (i->jobType == job.preferredJob && !i->busy) {
			job.villager = i;
			i->jobQueue.push(&job);
			i->busy = true;

			auto it = std::find(JobList.begin(), JobList.end(), &job);
			if (it != JobList.end()) {
				JobList.erase(it);
			}
			return;
		}
	}

	if (!bestVillager && job.preferredJob == JobType::None) {

		for (auto* i : Villager::allVillagers) {
			if (!i->busy) {
				job.villager = i;
				i->jobQueue.push(&job);
				i->busy = true;

				auto it = std::find(JobList.begin(), JobList.end(), &job);
				if (it != JobList.end()) {
					JobList.erase(it);
				}
				return;
			}
		}
	}
}


void Harvest::update() {
	std::pair<int, int> itemLocation;
	if (!itemFound) {
		itemLocation = findClosestTileItem(item, villager->xPos, villager->yPos);

		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);

		x = closestAdj.first;	
		y = closestAdj.second;

		itemFound = true;
	}


	if (villager->xPos == x && villager->yPos == y) {
		getTileRef(itemLocation.first, itemLocation.second).removeItem(item);
		villager->clock.restart();
		itemFound = false;
		completed = true;
	}
}

void HarvestTile::update() {

	Rule* rule = HarvestRuleRegistry::getInstance().get(item.name);

	// Check for required tool

	if (requiredTool) {
		if (villager->toolInHand != requiredTool) {
			std::cout << "Villager does not have required tool: " << requiredTool->name << std::endl;
			completed = true;
			return;
		}
	}

	if (!rule) {
		std::cout << "No harvest rule found for item: " << item.name << std::endl;
		completed = true;
		return;
	}

	x = locX;
	y = locY;

	std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);

	x = closestAdj.first;
	y = closestAdj.second;

	bool adjacent = isAtTile(villager->xPos, villager->yPos, locX, locY);

	if (adjacent) {
		if (!isHarvesting) {
			isHarvesting = true;
			villager->clock.restart();
		}

		if (villager->clock.getElapsedTime().asSeconds() > villager->harvestTime) {

			Tile& tile = getTileRef(locX, locY);
			Item* droppedItem = ItemRegistry::getInstance().get(rule->produces);

			tile.removeItem(item);

			for (int i = 0; i < rule->amount; i++) {
				itemsToMove.push_back({ droppedItem, {locX, locY} });
				tile.addItem(std::make_unique<Item>(*droppedItem));
			}
			

			villager->clock.restart();
			completed = true;
		}
	}
	else {
		isHarvesting = false;
	}
	
}

void Build::update() {
	x = locX;
	y = locY;

	Tile& tile = getTileRef(locX, locY);
	

	if (!tile.walkable) {
		completed = true;
		return;
	}
	
	std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);

	x = closestAdj.first;
	y = closestAdj.second;

	

	if (villager->xPos == x && villager->yPos == y) {
		tile.items.clear();

		tile.addItem(std::make_unique<Item>(itemToBuild));
		//tile.walkable = false;
		completed = true;
	}
}

void PlaceItem::update() {
	x = locX;
	y = locY;
	Tile& tile = getTileRef(locX, locY);
	std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);
	x = closestAdj.first;
	y = closestAdj.second;
	if (villager->xPos == x && villager->yPos == y) {
		tile.addItem(std::make_unique<Item>(itemToPlace));
		completed = true;
	}
}

void Idle::update() {

	/*float time = getRandomFloat(1.0f, 4.0f);
	int range = 5;
	if (villager->clock.getElapsedTime().asSeconds() > time) {
	    villager->clock.restart();
	    int newX = getRandomInt(villager->xPos - range, villager->xPos + range);
	    int newY = getRandomInt(villager->yPos - range, villager->yPos + range);

		x = newX;
		y = newY;
	}*/
}

void Plant::update() {

	bool needsSoil = true;

	// Update this to only allow planting on fertile land

	x = locX;
	y = locY;
	Tile& tile = getTileRef(locX, locY);

	if (!tile.walkable) {
		completed = true;
		return;
	}

	std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);

	x = closestAdj.first;
	y = closestAdj.second;

	Item soil = *ItemRegistry::getInstance().get("Soil");

	if (tile.containsItem(soil)) {
		needsSoil = false;
	}

	if (villager->xPos == x && villager->yPos == y) {
		if (needsSoil) {
			tile.addItem(std::make_unique<Item>(soil));
		}
		tile.addItem(std::make_unique<Crop>(crop));
		tile.animationType = NONE;
		completed = true;
	}
}

void Attack::update() {
	std::cout << "Attacking\n";
	// This doesnt need to find the closest target at all, a different system should assign targets. Just attack the assigned target.
	target = nullptr;
	float minDist = INFINITY;

	for (auto& c : Creature::allCreatures) {
		if (c == villager) continue;
		if (targetFilter && !targetFilter(c)) continue;
		float dist = pow(c->xPos - villager->xPos, 2) + pow(c->yPos - villager->yPos, 2);
		if (dist < minDist) {
			minDist = dist;
			target = c;
		}
	}

	if (!target) {
		std::cout << "No target found\n";
		completed = true;
		return;
	}

	if (target->dead) {
		std::cout << "Target already dead\n";
		completed = true;
		return;
	}

	float dist = sqrt(pow(target->xPos - villager->xPos, 2) + pow(target->yPos - villager->yPos, 2));


	float range = 1.0f;
	float attackCooldown = 1.0f;
	int dmg = 1;

	if (villager->itemInHand) {
		range = villager->itemInHand->getRange();
		attackCooldown = villager->itemInHand->getAttackCooldown();
		dmg = villager->itemInHand->getDamage();
	}

	if (dist > range) {	
		x = target->xPos;
		y = target->yPos;
	}

	else {

		x = villager->xPos;
		y = villager->yPos;

		if (villager->clock.getElapsedTime().asSeconds() > attackCooldown) {

			LightManager::addLight(glm::vec2(villager->xPixels, villager->yPixels), glm::vec3(1.0f, 1.0f, 0.0f), 300.0f, 1.5f, 0.02f);

			target->health -= dmg;
			villager->clock.restart();
		}
	}

	
}

void Retreat::update() {
	std::cout << "Retreating\n";
	// Safe score should take safe spaces, weapon in hand, personality, and distance into account
	int safeScore = -1;

	if (!threat || threat->dead) {
		completed = true;
		return;
	}

	float dx = villager->xPos - threat->xPos;
	float dy = villager->yPos - threat->yPos;

	float distance = sqrt(dx * dx + dy * dy);

	for (auto& i : getNeighbors(villager->xPos, villager->yPos)) {
		float ndx = i.first - threat->xPos;
		float ndy = i.second - threat->yPos;
		float newDist = sqrt(ndx * ndx + ndy * ndy);
		if (newDist > distance) {
			int score = (int)(newDist - distance);
			if (score > safeScore && getTileRef(i.first, i.second).walkable) {
				safeScore = score;
				x = i.first;
				y = i.second;

			}
		}
	}

	std::cout << "Distance to threat: " << distance << "\n";

	if (distance > 15) {
		completed = true;
	}

}

void Sleep::update() {
	x = villager->bed.first;
	y = villager->bed.second;

	if (villager->xPos == x && villager->yPos == y && !sleeping) {
		sleeping = true;
		villager->clock.restart();
		
	} else if (sleeping && villager->sleepTime < villager->clock.getElapsedTime().asSeconds()) {
		completed = true;
	}
}

void Craft::update() {

	if (!ingredients.empty()) {

		std::pair<int, int> itemLoc = findClosestTileItem(ingredients[0], villager->xPos, villager->yPos);
		std::pair<int, int> itemAdj = findClosestAdjTile(villager->xPos, villager->yPos, itemLoc.first, itemLoc.second);

		x = itemAdj.first;
		y = itemAdj.second;

		if (isAtTile(villager->xPos, villager->yPos, itemLoc.first, itemLoc.second)) {
			getTileRef(itemLoc.first, itemLoc.second).removeItem(ingredients[0]);
			ingredients.erase(ingredients.begin());
		}
	}
	else {

		std::pair<int, int> stationLoc = findClosestTileItem(*recipe->requiredStation, villager->xPos, villager->yPos);
		std::pair<int, int> stationAdj = findClosestAdjTile(villager->xPos, villager->yPos, stationLoc.first, stationLoc.second);

		x = stationAdj.first;
		y = stationAdj.second;

		if (isAtTile(villager->xPos, villager->yPos, stationLoc.first, stationLoc.second)) {
			std::unique_ptr<Item> resultItem = std::make_unique<Item>(recipe->result);
			auto ptr = resultItem.get();
			getTileRef(stationLoc.first, stationLoc.second).addItem(std::move(resultItem));
			itemsToMove.push_back({ ptr, {stationLoc.first, stationLoc.second} });
			completed = true;
		}
	}
}


void Move::update() {
	if (villager->xPos == x && villager->yPos == y) {
		completed = true;
	}
}

void MoveItem::update() {

	if (!itemPickedUp) {
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, fromX, fromY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (villager->xPos == x && villager->yPos == y) {
			getTileRef(fromX, fromY).removeItem(itemToMove);
			itemPickedUp = true;
		}
	} else {

		//auto goal = findClosestTileItem(stockPile, villager->xPos, villager->yPos);

		std::pair<int, int> goal = { toX, toY };

		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, goal.first, goal.second);
		x = closestAdj.first;
		y = closestAdj.second;
		if (villager->xPos == x && villager->yPos == y) {
			getTileRef(goal.first, goal.second).addItem(std::make_unique<Item>(itemToMove));
			std::cout << itemToMove.name << std::endl;
			completed = true;
		}
	}
}

bool isAtTile(int xPos, int yPos, int xLoc, int yLoc) {
    int dx = xPos - xLoc;
    int dy = yPos - yLoc;

	return std::abs(dx) + std::abs(dy) == 1;
}

std::pair<int, int> findClosestAdjTile(int xPos, int yPos, int xTile, int yTile) {
	float shortest = INFINITY;
	std::pair<int, int> closestAdj;
	for (auto& i : getNeighbors(xTile, yTile)) {
		float dx = i.first - xPos;
		float dy = i.second - yPos;
		float dist = (dx * dx) + (dy * dy);

		if (dist < shortest && getTileRef(i.first, i.second).walkable) {
			closestAdj = i;
			shortest = dist;
		}
	}

	return closestAdj;
}


/*

TO ADD:

Smithing/Crafting (needs crafting system first)
Moving items
Retreating
Guarding/Defending
Attacking (working on)
Building (working on)
Cooking (its just crafting ig?)

*/