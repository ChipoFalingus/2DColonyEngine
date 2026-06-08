#include "Job.h"
#include "Item.h"
#include "Light.h"
#include "CreatureUtils.h"
#include "Villager.h"
#include "Globals.h"
#include "HarvestRules.h"
#include "Game.h"
#include "Food.h"
#include "Seed.h"

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


		if (v->getCurrentJob()) continue;

		int score = 0;
		
		int skill = v->skills[job.type];
		score -= skill * 25;


		int dx = std::abs(v->xPos - job.x);
		int dy = std::abs(v->yPos - job.y);
		score += (dx + dy);

		if (score < bestScore) {
			bestScore = score;
			bestVillager = v;
		}
	}

	if (bestVillager) {
		job.villager = bestVillager;
		bestVillager->setCurrentJob(&job);
	}
}

void JobManager::findJobForColonist(Villager& v) {

	Job* bestJob = nullptr;
	int bestScore = std::numeric_limits<int>::max();

	for (auto& job : JobManager::JobList) {

		if (job->villager) continue;

		int score = 0;

		if (job->preferredTool && (!v.inventory.has(job->preferredTool->name)))
		{
			score += 1000;
		}
		
		int dx = std::abs(v.xPos - job->x);
		int dy = std::abs(v.yPos - job->y);

		score += dx + dy;
		score -= v.skills[job->type] * 500000;

		score += v.tiredness * 5;

		if (score < bestScore) {
			bestScore = score;
			bestJob = job;
		}
	}
	if (!bestJob) {
		return;
	}
	std::cout << "Assigned job to " << v.firstname << " " << v.lastname << " with score " << bestScore << std::endl;
	bestJob->villager = &v;
	v.setCurrentJob(bestJob);
	//JobManager::removeJob(bestJob);
}

struct Bid {
	Job* job;
	Villager* villager;
	int score;
};;

void JobManager::assignJobs() {
	
	std::vector<Bid> bids;

	for (Villager* v : mainWorld.getAllVillagers())
	{
		if (!v) continue;

		for (Job* job : JobList)
		{
			if (!job)
				continue;

			if (job->state != JobState::Queued)
				continue;

			int score = 0;

			int dx = std::abs(v->xPos - job->x);
			int dy = std::abs(v->yPos - job->y);

			score += dx + dy;
			score -= v->skills[job->type] * 5000;

			score += v->tiredness * 5;

			if (v->getCurrentJob()) score += 1000;

			bids.push_back({ job, v, score });
		}
	}

	std::sort(bids.begin(), bids.end(), [](const Bid& a, const Bid& b) {
		return a.score < b.score;
		});

	std::unordered_set<Job*> assignedJobs;
	std::unordered_set<Villager*> assignedVillagers;

	for (auto& bid : bids) {
		// Skip job if its already assigned
		if (assignedJobs.count(bid.job))
			continue;


		assignedJobs.insert(bid.job);

		if (bid.villager->getCurrentJob())
			continue;

		std::cout << "Assigned job to " << bid.villager->firstname << " " << bid.villager->lastname << " with score " << bid.score << std::endl;

		bid.villager->setCurrentJob(bid.job);

		bid.job->villager = bid.villager;
		bid.job->state = JobState::Active;

		assignedVillagers.insert(bid.villager);
	}
}

void JobManager::update() {
	for (Job* job : JobList) {
		if (!job) continue;
		if (job->state == JobState::Waiting) {
			job->waitingUpdate();
		}
	}

	assignJobs();

	for (size_t i = 0; i < JobList.size(); )
	{
		Job* job = JobList[i];

		if (!job)
		{
			i++;
			continue;
		}

		if (job->state == JobState::Completed ||
			job->state == JobState::Failed)
		{
			if (job->villager &&
				job->villager->getCurrentJob() == job)
			{
				job->villager->setCurrentJob(nullptr);
			}

			delete job;
			JobList.erase(JobList.begin() + i);
		}
		else
		{
			i++;
		}
	}
}


void Harvest::update() {
	std::optional<std::pair<int, int>> itemLocation;
	if (!itemFound) {
		itemLocation = findClosestTileItem(item->name, villager->xPos, villager->yPos);

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
	Tile& tile = getTileRef(locX, locY);
	
	if (!villager) {
		state = JobState::Waiting;
		return;
	}

	// Check for required tool
	//if (!toolsMatch(preferredToolName, villager->toolInHand.get()) && !addedGetToolJob) {
	if (!villager->inventory.has(rule->toolRequired) && rule->toolRequired != "None") {
		addedGetToolJob = true;
		//harvestState = HarvestTile::GrabbingTool;
	}


	switch (harvestState)
	{
	case HarvestTile::GrabbingTool: {
		auto itemLocation = findClosestItemType(villager->xPos, villager->yPos, 50, [&](const Object& item, int x, int y) {
			return item.name == rule->toolRequired && !item.claimed;
			});
		if (!itemLocation) {
			state = JobState::Waiting;
			//villager->removeJob(this);
			villager->setCurrentJob(nullptr);
			return;
		}

		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, itemLocation->x, itemLocation->y);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(villager->xPos, villager->yPos, itemLocation->x, itemLocation->y)) {
			villager->pickUpItem(itemLocation->item.lock(), itemLocation->x, itemLocation->y);
			harvestState = HarvestTile::MovingToTile;
		}

		break;
	}
	case HarvestTile::MovingToTile: {
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, locX, locY);

		x = closestAdj.first;
		y = closestAdj.second;

		bool adjacent = isAtTile(villager->xPos, villager->yPos, locX, locY);

		if (adjacent) {
			villager->clock = 0.0f;
			harvestState = HarvestTile::Harvesting;
		}

		break;
	}
	case HarvestTile::Harvesting: {
		int skill = villager->skills[type];
		if (villager->clock > villager->harvestTime * (10.0f / skill)) {

			auto targetItem = ObjectRegistry::getInstance().get(rule->target);
			tile.removeItem(targetItem, locX, locY);

			for (auto& i : rule->produces) {
				auto droppedItem = ObjectRegistry::getInstance().get(i);
				tile.addObject(droppedItem);
				mainWorld.addItemToMove(droppedItem, locX, locY);
			}

			villager->clock = 0.0f;
			//villager->tiredness += 5;

			//auto i = static_cast<Tool*>(villager->inventory.get(rule->toolRequired).get());
			//i->durability--;

			/*if (i->durability <= 0) {
				std::cout << "Tool broke: " << i->name << std::endl;
				villager->inventory.remove(rule->toolRequired);
			}*/

			getTileRef(locX, locY).markedForHarvest = false;
			state = JobState::Completed;
		}
	}
		break;
	default: 
		break;
	}
	
}

void HarvestTile::waitingUpdate() {

	if (rule->toolRequired == "None" || rule->toolRequired.empty()) {
		harvestState = HarvestTile::MovingToTile;
		state = JobState::Queued;
		return;
	}

	for (auto* v : mainWorld.getAllVillagers()) {
		if (!v) continue;
		if (v->inventory.has(rule->toolRequired)) {
			state = JobState::Queued;
			return;
		}
	}

	searchTimer += Clock::deltaTime;
	if (searchTimer < 3.0f) {
		return;
	}
	searchTimer = 0.0f;

	auto itemLocation = findClosestItemType(locX, locY, 50, [&](const Object& item, int x, int y) {
		return item.name == rule->toolRequired/* && !item.claimed*/;
		});

	if (itemLocation) {
		state = JobState::Queued;
		std::cout << "Tool found for harvest job, making job available for assignment." << std::endl;
	}
}

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

void BuildFurniture::update() {

	switch (jobState)
	{
	case BuildFurniture::Getting: {

		auto closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, fX, fY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(villager->xPos, villager->yPos, fX, fY)) {
			villager->pickUpItem(itemName.lock(), fX, fY);
			jobState = BuildFurniture::Placing;
		}
		break;
	}

	case BuildFurniture::Placing: {
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (isAtTile(villager->xPos, villager->yPos, tX, tY)) {
			itemName.lock()->claimed = false;
			villager->dropItem(itemName.lock(), tX, tY);
			state = JobState::Completed;

			break;

		}
	}
	default:
		break;
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

	Tile& tile = getTileRef(locX, locY);

	if (!tile.walkable) {
		state = JobState::Completed;
		return;
	}

	switch (plantState)
	{
	case Plant::GettingSeed: {
		auto itemLocation = findClosestItemType(villager->xPos, villager->yPos, 50, [&](const Object& item, int x, int y) {
			return item.name == seed && !item.claimed;
			});
		if (!itemLocation) {
			state = JobState::Completed;
			return;
		}
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, itemLocation->x, itemLocation->y);
		x = closestAdj.first;
		y = closestAdj.second;
		if (isAtTile(villager->xPos, villager->yPos, itemLocation->x, itemLocation->y)) {
			villager->pickUpItem(itemLocation->item.lock(), itemLocation->x, itemLocation->y);
			plantState = Plant::Planting;
		}
		break;
	}
	case Plant::Planting: {
		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, locX, locY);

		x = closestAdj.first;
		y = closestAdj.second;

		if (tile.type == tileType::SOIL) {
			needsSoil = false;
		}

		if (villager->xPos == x && villager->yPos == y) {
			if (needsSoil) {
				tile.changeTileType(tileType::SOIL);
			}
			auto c = ObjectRegistry::getInstance().get(seed);
			if (c->type != Type::Seed) {
				state = JobState::Completed;
				return;
			}
			auto s = static_cast<Seed*>(c.get());
			villager->inventory.remove(seed);
			tile.addObject(s->cropType);
			tiles.push_back({ locX, locY });
			state = JobState::Completed;
		}

		break;

	}
	default:
		break;
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

	if (!villager->threat || villager->threat->dead) {
		state = JobState::Completed;
		return;

	}



	float dx = villager->xPos - villager->threat->xPos;

	float dy = villager->yPos - villager->threat->yPos;



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



		score += (villager->xPos + villager->yPos) / 10.0f;



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



	if (!villager->threat || villager->threat->dead) {

		//std::cout << "Retreated" << std::endl;

		state = JobState::Completed;

	}

}



void Sleep::update() {

	if (!villager->bed && !lookedForBed) {
		villager->claimBed();
		lookedForBed = true;
	}
	
	if (villager->bed) {
		x = villager->bed->first;
		y = villager->bed->second;
	}
	else {
		x = villager->xPos;
		y = villager->yPos;
	}

	if (villager->xPos == x && villager->yPos == y && !sleeping) {
		sleeping = true;
		villager->clock = 0.0f;
		
	} else if (sleeping && mainWorld.dayCycle.getTimePeriod() == TimePeriod::Morning) {
		sleeping = false;
		villager->tiredness = 0;
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
		if (!findClosestTileItem(stationItem->name, villager->xPos, villager->yPos)) {
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
		auto loc = findClosestTileItem(item->name, villager->xPos, villager->yPos);

		auto adjLoc = findClosestAdjTile(villager->xPos, villager->yPos, loc->first, loc->second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(villager->xPos, villager->yPos, loc->first, loc->second)) {

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
			itemToMove->claimed = false;

			auto* s = mainWorld.atStockpile(toX, toY);
			if (s) {
				s->placeItem(itemToMove, toX, toY);
			}


			state = JobState::Completed;
		}
	}
}

void FindFood::update() {
	/*if (!food) {
		completed = true;
		return;
	}*/

	/*if (villager->hunger > 10) {
		food->claimed = false;
		state = JobState::Completed;
		return;
	}*/

	switch (foodState) {

	case State::Grab: {
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

			getTileRef(tX, tY).removeItem(food, tX, tY);

			foodState = State::Find;
		}

		break;
	}
		

	case State::Find: {
		place = findClosestItemType(villager->xPos, villager->yPos, 200, [](const Object& obj, int x, int y) {
			if ((obj.name != "Wooden Chair") || obj.claimed) {
				return false;
			}

			static const std::pair<int, int> dirs[4] = {
				{0,1},{0,-1},{1,0},{-1,0}
			};

			for (auto& d : dirs) {
				int nx = x + d.first;
				int ny = y + d.second;

				const Tile& t = getTileRef(nx, ny);

				for (auto& item : t.items) {
					if (item->name == "Wooden Table") {
						return true;
					}
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

	case State::Eat: {
		if (place.has_value()) {
			x = place->x;
			y = place->y;
		}
		else {
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

void Meditate::update() {
	if (!hasTarget) {
		x = villager->xPos;
		y = villager->yPos;
		hasTarget = true;
	}
	if (villager->xPos == x && villager->yPos == y) {
		if (villager->clock > 15.0f) {
			villager->tiredness = std::max(0, villager->tiredness - 20);
			state = JobState::Completed;
		}
	}
}

void Talk::update() {
	if (!other) {
		state = JobState::Completed;
		return;
	}
	x = other->xPos;
	y = other->yPos;
	if (isAtTile(villager->xPos, villager->yPos, x, y)) {
		if (villager->clock > 10.0f) {
			//villager->friendliness[other] += 10;
			villager->social = 0;
			state = JobState::Completed;
		}
	}
}

std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>>
findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients) {

	std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Object>>> result;
	std::vector<std::shared_ptr<Object>> reserved;
	bool success = true;

	for (const auto& [name, quantity] : ingredients) {
		std::cout << "Finding ingredient: " << name << " x" << quantity << std::endl;
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