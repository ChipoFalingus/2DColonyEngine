#include "Job.h"
#include "Item.h"
#include "Light.h"
#include "CreatureUtils.h"
#include "Villager.h"
#include "Globals.h"
#include "HarvestRules.h"
#include "Game.h"
#include "Food.h"

#include <limits>
#include "World.h"

std::vector<Job*> JobManager::JobList;
bool isAtTile(int xPos, int yPos, int xLoc, int yLoc);
std::pair<int, int> findClosestAdjTile(int xPos, int yPos, int xTile, int yTile);
std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>>
findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients);

static bool toolsMatch(const std::string& required, Tool* have) {
	if (required.empty()) return true;
	if (!have) return false;       
	return required == have->name;
}

void JobManager::findBestColonistForJob(Job& job) {

	Villager* bestVillager = nullptr;

	int bestScore = std::numeric_limits<int>::max();

	for (auto* v : mainWorld.getAllVillagers()) {

		int score = 0;

		if (job.preferredTool) {
			if (!toolsMatch(job.preferredToolName, v->toolInHand.get())) {
				continue;
			}
		}

		if (job.type != JobType::None) {
			if (v->getJob() != job.type) {
				continue;
			}

		}
		
		int dx = std::abs(v->xPos - job.x);
		int dy = std::abs(v->yPos - job.y);
		score += (dx + dy);
		score += v->getJobQueueSize() * 100;

		if (score < bestScore) {
			bestScore = score;
			bestVillager = v;
		}
	}

	if (bestVillager) {
		job.villager = bestVillager;
		bestVillager->addToJobQueue(&job);

		JobManager::removeJob(&job);
	}
}

//void JobManager::findJobForColonist(Villager& v) {
//
//	Job* bestJob = nullptr;
//	int bestScore = std::numeric_limits<int>::max();
//
//	for (auto& job : JobManager::JobList) {
//
//		if (job->taken) continue;
//
//		int score = 0;
//
//		if (job->preferredTool && (!v.toolInHand ||
//			v.toolInHand->name != job->preferredTool->name))
//		{
//			continue;
//		}
//		
//		if (job->preferredJob != JobType::None && v.getJob() != job->preferredJob) {
//			continue;
//		}
//		
//		int dx = std::abs(v.xPos - job->x);
//		int dy = std::abs(v.yPos - job->y);
//
//		score = dx + dy;
//
//		//score += v.getJobQueueSize() * 10000;
//
//		if (score < bestScore) {
//			bestScore = score;
//			bestJob = job;
//		}
//	}
//	if (!bestJob) {
//		return;
//	}
//	std::cout << "Assigned job to " << v.firstname << " " << v.lastname << " with score " << bestScore << std::endl;
//	bestJob->villager = &v;
//	v.addToJobQueue(bestJob);
//	JobManager::removeJob(bestJob);
//}


void Harvest::update() {
	std::optional<std::pair<int, int>> itemLocation;
	if (!itemFound) {
		itemLocation = findClosestTileItem(*item.get(), villager->xPos, villager->yPos);

		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);

		x = closestAdj.first;	
		y = closestAdj.second;

		itemFound = true;
	}

	if (villager->xPos == x && villager->yPos == y) {
		getTileRef(itemLocation->first, itemLocation->second).removeItem(item, itemLocation->first, itemLocation->second);
		itemFound = false;
		state = JobState::Completed;
	}
}

void HarvestTile::update() {
	Rule* rule = HarvestRuleRegistry::getInstance().get(item);
	Tile& tile = getTileRef(locX, locY);

	// Check for required tool
	if (!toolsMatch(preferredToolName, villager->toolInHand.get())) {
		std::cout << "Tool requirement not met for harvesting " << item << std::endl;
		state = JobState::Completed;
		return;
	}

	if (!rule) {
		std::cout << "No harvest rule found for item: " << item << std::endl;
		state = JobState::Completed;
		return;
	}

	villager->activity_state = ActivityState::Working;

	x = locX;
	y = locY;

	std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);

	x = closestAdj.first;
	y = closestAdj.second;

	bool adjacent = isAtTile(villager->xPos, villager->yPos, locX, locY);

	if (adjacent) {

		if (!isHarvesting) {
			isHarvesting = true;
			villager->clock = 0.0f;
		}

		if (villager->clock > villager->harvestTime) {

			auto targetItem = ObjectRegistry::getInstance().get(rule->target);
			tile.removeItem(targetItem, locX, locY);

			for (auto& i : rule->produces) {
				auto droppedItem = ObjectRegistry::getInstance().get(i);
				tile.addObject(droppedItem);
				mainWorld.addItemToMove(droppedItem, locX, locY);
			}
			
			villager->clock = 0.0f;
			villager->tiredness += 5;

			getTileRef(locX, locY).markedForHarvest = false;
			state = JobState::Completed;
		}
	}
	else {
		isHarvesting = false;
	}
	
}

// Should check if all items are in stockpile instead of grabbing one by one
// Nice job team!
void Build::update() {


	if (!init) {
		reserve = findIngredientsForJob(ingredients);
		init = true;
		if (reserve.empty()) {
			state = JobState::Completed;
			return;
		}
	}

	if (!grabbedAllItems) {
		auto& itemObj = reserve.front();

		int xPos = itemObj.first.first;
		int yPos = itemObj.first.second;

		auto adjLoc = findClosestAdjTile(villager->xPos, villager->yPos, xPos, yPos);
		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(villager->xPos, villager->yPos, xPos, yPos)) {
			if (auto s = mainWorld.atStockpile(xPos, yPos)) {
				villager->pickUpItem(itemObj.second, xPos, yPos, s);
			}

			reserve.erase(reserve.begin());

			if (reserve.empty()) {
				grabbedAllItems = true;
			}
		}
	}
	else {
		std::pair<int, int> loc = { locX, locY };

		auto adjLoc = findClosestAdjTile(villager->xPos, villager->yPos, loc.first, loc.second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(villager->xPos, villager->yPos, loc.first, loc.second)) {

			for (int i = 0; i < recipe->quantity; i++) {
				getTileRef(loc.first, loc.second).addObject(recipe->result);
				//getTileRef(loc.first, loc.second).walkable = false;
			}

			state = JobState::Completed;
		}
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
		tile.addObject(std::make_unique<Object>(*itemToPlace));
		state = JobState::Completed;
	}
}

void Idle::update() {

	int range = 5;

	if (!initialized) {
		for (int i = 0; i < 10; i++) {
			int nx = getRandomInt(villager->xPos - range, villager->xPos + range);
			int ny = getRandomInt(villager->yPos - range, villager->yPos + range);

			if (getTileRef(nx, ny).walkable) {
				x = nx;
				y = ny;
				break;
			}
		}

		initialized = true;
	}

	if (isAtTile(villager->xPos, villager->yPos, x, y)) {
		state = JobState::Completed;
	}
}

void Plant::update() {

	bool needsSoil = true;

	// Update this to only allow planting on fertile land

	Tile& tile = getTileRef(locX, locY);

	if (!tile.walkable) {
		state = JobState::Completed;
		return;
	}

	std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, locX, locY);

	x = closestAdj.first;
	y = closestAdj.second;

	if (tile.containsItem("Soil")) {
		needsSoil = false;
	}

	if (villager->xPos == x && villager->yPos == y) {
		if (needsSoil) {
			tile.addObject("Soil");
		}
		tile.addObject(name);
		state = JobState::Completed;
	}
}

void Attack::update() {

	if (!target || target->dead) {
		auto c = findClosestCreatureType<Zombie>(villager->xPos, villager->yPos, villager->alertness * 1.5f);
		if (c) {
			target = c;
		} else {
			x = villager->xPos;
			y = villager->yPos;
			state = JobState::Completed;
			return;
		}
	}

	float range;
	float attackCooldown;
	int dmg;


	if (villager->itemInHand) {
		if (villager->itemInHand->type == Type::Gun) {
			Gun* i = static_cast<Gun*>(villager->itemInHand.get());
			range = i->getRange();
			attackCooldown = i->getAttackCooldown();
			dmg = i->getDamage();
		}
		else {
			range = 1.0f;
			attackCooldown = 1.0f;
			dmg = 1;
		}
	} else {
		range = 1.0f;
		attackCooldown = 1.0f;
		dmg = 1;
	}

	float dx = target->xPos - villager->xPos;
	float dy = target->yPos - villager->yPos;
	float distSq = dx * dx + dy * dy;
	float rangeSq = range * range;

	if (distSq > rangeSq) {
		// Move toward target (replace later with pathfinding)
		x = target->xPos;
		y = target->yPos;
	}
	else {
		// Stay in place
		x = villager->xPos;
		y = villager->yPos;

		if (villager->clock > attackCooldown) {
			target->takeDamage(dmg, villager);

			float dx = target->xPos - villager->xPos;
			float dy = target->yPos - villager->yPos;

			float length = std::sqrt(dx * dx + dy * dy);

			glm::vec2 dir;
			if (length == 0) {
				dir = glm::vec2(1.0f, 0.0f);
			}
			else {
				 dir = glm::vec2(dx / length, dy / length);
			}

			Game::getInstance().getLightManager().addDLight(glm::vec2(villager->xPos, villager->yPos), glm::vec3(1.0f, 1.0f, 0.0f), 10.0f, 5.0f, 0.03f, dir);
			villager->clock = 0.0f;;
		}
	}
}

void Retreat::update() {
	// Sees if there is cover nearby (if they dont have a weapon, this part isnt important)

	// Sees if there are safe spaces (basically looking for cover for colonists with no weapon)

	// Sees if there are nearby allies with good weapons and health (tune to make colonists not so clingy, also this is a last last last resort)
	
	// Extra stuff:
		// Could have them search for a weapon to join the fight

	
	int safeScore = -1;

	if (!threat || threat->dead) {
		state = JobState::Completed;
		return;
	}

	float dx = villager->xPos - threat->xPos;
	float dy = villager->yPos - threat->yPos;

	float distance = sqrt(dx * dx + dy * dy);

	int dim = 64;

	auto threatMap = buildThreatMap(villager->xPos, villager->yPos, dim);

	int startX = villager->xPos - dim / 2;
	int startY = villager->yPos - dim / 2;

	float bestScore = 99999.0f;
	std::pair<int, int> bestMove = { 0, 0 };

	std::vector<std::pair<int, int>> dirs = {
		{1,0}, {0,1}, {-1,0}, {0,-1}
	};

	std::shuffle(dirs.begin(), dirs.end(), rng);

	bool foundMove = false;

	int cx = villager->xPos - startX;
	int cy = villager->yPos - startY;

	for (auto& i : dirs) {
		int nx = villager->xPos + i.first;
		int ny = villager->yPos + i.second;

		int fx = nx - startX;
		int fy = ny - startY;

		if (fx < 0 || fy < 0 || fx >= dim || fy >= dim)
			continue;

		if (!getTileRef(nx, ny).walkable)
			continue;

		float score = threatMap[fx][fy];

		if (villager->lastMove != i) {
			score += 0.5f;
		}

		if (score < bestScore) {
			bestScore = score;
			bestMove = i;
			foundMove = true;
		}
	}

	float stayScore = threatMap[cx][cy];
	
	if (stayScore < bestScore) {
		bestMove = { 0,0 };
	}

	if (foundMove && villager->moveClock > villager->speed) {
		villager->xPos += bestMove.first;
		villager->yPos += bestMove.second;

		villager->lastMove = bestMove;
		villager->moveClock = 0.0f;
	}

	if (distance > 15) {
		villager->threat = nullptr;
		//std::cout << "Retreated" << std::endl;
		state = JobState::Completed;
	}
}

void Sleep::update() {

	x = villager->bed.first;
	y = villager->bed.second;

	if (!villager->bed.first && !villager->bed.second) {
		x = villager->xPos;
		y = villager->yPos;
	}

	if (villager->xPos == x && villager->yPos == y && !sleeping) {
		sleeping = true;
		villager->clock = 0.0f;
		
	} else if (sleeping && villager->sleepTime < villager->clock) {
		sleeping = false;
		state = JobState::Completed;
	}
}

std::vector<std::string> getIngredientList(const std::string& itemToCraft);


// Should check if all ingredients are in stockpile instead of grabbing one by one
// Also needs to be able to handle multiple steps of crafting (crafting subcomponents first)

void Craft::update() {

	if (!recipe) {
		std::cout << "If you see this something has gone horribly wrong :(\nSOURCE: CRAFTING JOB" << itemName << std::endl;
		state = JobState::Completed;
		return;
	}

	if (recipe->requiredStation != "None") {
		auto stationItem = ObjectRegistry::getInstance().get(recipe->requiredStation);
		if (!findClosestTileItem(*stationItem, villager->xPos, villager->yPos)) {
			std::cout << "Required station not found: " << stationItem->name << std::endl;
			state = JobState::Completed;
			return;
		}
	}

	if (!init) {
		reserve = findIngredientsForJob(ingredients);
		init = true;
		if (reserve.empty()) {
			state = JobState::Completed;
			return;
		}
	}



	if (!grabbedAllItems) {
		// Find the first ingredient in stockpile
		auto itemObj = reserve[0];

		auto adjLoc = findClosestAdjTile(villager->xPos, villager->yPos, itemObj.first.first, itemObj.first.second);
		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(villager->xPos, villager->yPos, itemObj.first.first, itemObj.first.second)) {
			if (auto s = mainWorld.atStockpile(itemObj.first.first, itemObj.first.second)) {
				villager->pickUpItem(itemObj.second, itemObj.first.first, itemObj.first.second, s);
			}

			reserve.erase(reserve.begin());

			if (reserve.empty()) {
				grabbedAllItems = true;
			}
		}
	} else {
		auto item = ObjectRegistry::getInstance().get(recipe->requiredStation);
		auto loc = findClosestTileItem(*item, villager->xPos, villager->yPos);

		auto adjLoc = findClosestAdjTile(villager->xPos, villager->yPos, loc->first, loc->second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(villager->xPos, villager->yPos, loc->first, loc->second)) {

			if (item->name == "Carpentry Bench") {
			}
			for (int i = 0; i < recipe->quantity; i++) {
				auto item = ObjectRegistry::getInstance().get(recipe->result);
				if (!item) {
					std::cout << "Registry returned nullptr for " << recipe->result << std::endl;
					state = JobState::Completed;
					return;
				}

				if (item->type == Type::Tool) {
					auto tool = static_cast<Tool*>(item.get());
					tool->material = recipe->material;
				}
				else if (item->type == Type::Food) {
					auto food = static_cast<Food*>(item.get());
				}

				getTileRef(loc->first, loc->second).addObject(item);
				mainWorld.addItemToMove(item, loc->first, loc->second);
			}
			
			state = JobState::Completed;
		}
	}
}

std::vector<std::string> getIngredientList(const std::string& itemToCraft) {
	std::vector<std::string> result;

	Recipe* recipe = RecipeRegistry::getInstance().get(itemToCraft);
	if (!recipe) {
		result.push_back(itemToCraft);
		return result;
	}
	for (auto& [name, count] : recipe->ingredients) {
		for (int i = 0; i < count; i++) {
			auto foundItem = mainWorld.findItemInAllStockpile(name);
			if (!foundItem) {
				std::vector<std::string> subResult = getIngredientList(name);
				result.insert(std::end(result), std::begin(subResult), std::end(subResult));
			}
			else {
				result.push_back(name);
			}
		}
	}

	return result;
}


void Move::update() {
	x = toX;
	y = toY;
	if (villager->xPos == toX && villager->yPos == toY) {
		state = JobState::Completed;
	}
}

void MoveItem::update() {

	villager->activity_state = ActivityState::Working;

	if (!itemPickedUp) {
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, fromX, fromY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (villager->xPos == x && villager->yPos == y) {
			villager->pickUpItem(itemToMove, fromX, fromY);
			itemPickedUp = true;
		}
	} else {

		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, toX, toY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (villager->xPos == x && villager->yPos == y) {

			villager->dropItem(itemToMove, toX, toY);
			state = JobState::Completed;
		}
	}
}

void FindFood::update() {
	/*if (!food) {
		completed = true;
		return;
	}*/

	if (villager->hunger > 10) {
		food->claimed = false;
		state = JobState::Completed;
		return;
	}

	switch (foodState) {

	case State::Grab:
	{
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(villager->xPos, villager->yPos, tX, tY)) {
			Tile& tile = getTileRef(tX, tY);
			if (!tile.containsItem(food->name)) {
				food->claimed = false;
				state = JobState::Completed;
				return;
			}
			//auto obj = ObjectRegistry::getInstance().get(food->name);

			//villager->pickUpItem(obj, tX, tY);
			getTileRef(tX, tY).removeItem(food, tX, tY);
			//mainWorld.removeItemToMove(food, tX, tY);


			foodState = State::Find;
		}

		break;
	}
		

	case State::Find:
	{
		place = findClosestItemType(villager->xPos, villager->yPos, 50, [](const Object& obj, int x, int y) {
			if ((obj.name != "Wooden Chair" && obj.name != "Stone Chair") || obj.claimed)
				return false;

			static const std::pair<int, int> dirs[4] = {
				{0,1},{0,-1},{1,0},{-1,0}
			};

			for (auto& d : dirs) {
				int nx = x + d.first;
				int ny = y + d.second;

				const Tile& t = getTileRef(nx, ny);

				for (auto& item : t.items) {
					if (item->name == "Wooden Table" || item->name == "Stone Table")
						return true;
				}
			}

			return false;

			});
		if (place.has_value()) {
			place->item.lock()->claimed = true;
		}
		foodState = State::Eat;
		std::cout << "Find: " << food->getNutrition() << std::endl;
		break;
	}

	case State::Eat:
	{
		if (place.has_value()) {
			x = place->x;
			y = place->y;
		}
		else {
			std::cout << "Table + Chair not found" << std::endl;
			x = villager->xPos;
			y = villager->yPos;
		}

		if (villager->xPos == x && villager->yPos == y) {
			
			eatTimer += Clock::deltaTime;

			if (eatTimer > 10.0f) {

				villager->hunger += food->getNutrition();
				
				//villager->inventory.remove(food->name, 1);
				villager->isHungry = false;

				if (place.has_value()) {
					place->item.lock()->claimed = false;
				}
				villager->activity_state = ActivityState::None;
				state = JobState::Completed;
			}
		}

		break;
	}
	}
}

void Sit::update() {
	x = tX;
	y = tY;

	if (init && villager->activity_state != ActivityState::Sitting) {
		state = JobState::Completed;
		return;
	}

	if (villager->xPos == tX && villager->yPos == tY) {
		villager->activity_state = ActivityState::Sitting;
		init = true;
	}
}

Sit::~Sit() {

	if (villager) {
		villager->activity_state = ActivityState::None;
	}

	if (villager && villager->object_in_use) {
		villager->object_in_use->claimed = false;
		villager->object_in_use = nullptr;
	}
}

void Wander::update() {

	if (!hasTarget) {
		pickNewTarget();

		if (!hasTarget) {
			std::cout << "no" << std::endl;
			state = JobState::Completed;
			return;
		}
	}

	x = tX;
	y = tY;

	if (villager->xPos == tX && villager->yPos == tY) {

		state = JobState::Completed;
	}
}

void Wander::pickNewTarget() {

	int range = 6;

	for (int i = 0; i < 10; i++) {

		int nx = villager->xPos + getRandomInt(-range, range);
		int ny = villager->yPos + getRandomInt(-range, range);

		if (!getTileRef(nx, ny).walkable)
			continue;

		tX = nx;
		tY = ny;

		hasTarget = true;

		return;
	}

	hasTarget = false;
}

std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>>
findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients) {

	std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>> result;
	std::vector<std::shared_ptr<Object>> reserved;
	bool success = true;

	for (const auto& [name, quantity] : ingredients) {

		for (int i = 0; i < quantity; i++) {
			auto itemLocation = mainWorld.findUnclaimedItemInAllStockpile(name);
			if (!itemLocation) {
				std::cout << "Ingredient " << name << " not found in any stockpile\n";
				success = false;
				break;
			}
			itemLocation->second->claimed = true;
			reserved.push_back(itemLocation->second);
			result.push_back(*itemLocation);
		}
		if (!success) {
			break;
		}
	}

	if (!success) {
		for (std::shared_ptr<Object> obj : reserved) {
			obj->claimed = false;
		}
		return {};
	}

	return result;
}


bool isAtTile(int xPos, int yPos, int xLoc, int yLoc) {
    int dx = xPos - xLoc;
    int dy = yPos - yLoc;

	return std::abs(dx) + std::abs(dy) == 1;
}

bool isOnTile(int xPos, int yPos, int xLoc, int yLoc) {
	return xPos == xLoc && yPos == yLoc;
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