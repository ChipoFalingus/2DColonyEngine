#include "Jobs/Job.h"
#include "Utility/CreatureUtils.h"
#include "Utility/Globals.h"
#include "Game.h"
#include "Utility/ItemLocation.h"

#include "Entities/CreatureComponents.h"

#include <limits>
#include "World/World.h"
#include "Utility/ItemUtils.h"

std::vector<Job*> JobManager::JobList;
bool isAtTile(int xPos, int yPos, int xLoc, int yLoc);
std::pair<int, int> findClosestAdjTile(int xPos, int yPos, int xTile, int yTile);
std::vector<std::pair<std::pair<int, int>, entt::entity>> findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients);

//static bool toolsMatch(const std::string& required, Tool* have) {
//	if (required.empty()) return true;
//	if (!have) return false;       
//	return required == have->name;
//}
//
//void JobManager::findBestColonistForJob(Job& job) {
//
//	Villager* bestVillager = nullptr;
//
//	int bestScore = std::numeric_limits<int>::max();
//
//	for (auto* v : mainWorld.getAllVillagers()) {
//
//
//		if (v->getCurrentJob()) continue;
//
//		int score = 0;
//		
//		int skill = v->skills[job.type];
//		score -= skill * 25;
//
//
//		int dx = std::abs(v->xPos - job.x);
//		int dy = std::abs(v->yPos - job.y);
//		score += (dx + dy);
//
//		if (score < bestScore) {
//			bestScore = score;
//			bestVillager = v;
//		}
//	}
//
//	if (bestVillager) {
//		job.villager = bestVillager;
//		bestVillager->setCurrentJob(&job);
//	}
//}
//
//void JobManager::findJobForColonist(Villager& v) {
//
//	Job* bestJob = nullptr;
//	int bestScore = std::numeric_limits<int>::max();
//
//	for (auto& job : JobManager::JobList) {
//
//		if (job->villager) continue;
//
//		int score = 0;
//
//		if (job->preferredTool && (!v.inventory.has(job->preferredTool->name)))
//		{
//			score += 1000;
//		}
//		
//		int dx = std::abs(v.xPos - job->x);
//		int dy = std::abs(v.yPos - job->y);
//
//		score += dx + dy;
//		score -= v.skills[job->type] * 500000;
//
//		score += v.tiredness * 5;
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
//	v.setCurrentJob(bestJob);
//	//JobManager::removeJob(bestJob);
//}

struct Bid {
	Job* job;
	entt::entity villager;
	int score;
};

void JobManager::assignJobs() {

	std::vector<Bid> bids;

	auto& registry = mainWorld.registry;
	auto view = registry.view<Villager, JobComponent, Position, Skills>();

	for (auto [entity, job_component, position, skills] : view.each()) {

		for (Job* job : JobList) {
			if (!job)
				continue;

			if (job->state != JobState::Queued)
				continue;

			int score = 0;

			int dx = std::abs(position.x - job->x);
			int dy = std::abs(position.y - job->y);

			score += dx + dy;
			score -= skills.getSkillLevel(job->type) * 5000;
			//score += v->tiredness * 5;

			if (job_component.currentJob) score += 100;

			bids.push_back({ job, entity, score });
		}
	}

	std::sort(bids.begin(), bids.end(), [](const Bid& a, const Bid& b) {
		return a.score < b.score;
		});

	std::unordered_set<Job*> assignedJobs;
	std::unordered_set<entt::entity> assignedVillagers;

	for (auto& bid : bids) {
		// Skip job if its already assigned
		if (assignedJobs.count(bid.job))
			continue;


		assignedJobs.insert(bid.job);

		auto villager = registry.try_get<JobComponent>(bid.villager);

		if (villager->currentJob)
			continue;

		auto name = registry.try_get<Name>(bid.villager);

		villager->currentJob = bid.job;

		bid.job->villager = bid.villager;
		bid.job->state = JobState::Active;

		assignedVillagers.insert(bid.villager);
	}
}

void JobManager::update() {
	for (Job* job : JobList) {
		if (!job) continue;
		if (job->state == JobState::Waiting) {
			//job->waitingUpdate();
		}
	}

	assignJobs();

	for (size_t i = 0; i < JobList.size(); ) {
		Job* job = JobList[i];

		if (!job) {
			i++;
			continue;
		}

		if (job->state == JobState::Completed ||
			job->state == JobState::Failed) {

			auto jobComponent = mainWorld.registry.try_get<JobComponent>(job->villager);
			if (!jobComponent) {
				job->villager = entt::null;
			}
			else if (jobComponent->currentJob == job) {
				jobComponent->currentJob = nullptr;
			}

			//delete job;
			JobList.erase(JobList.begin() + i);
		}
		else {
			i++;
		}
	}
}


//void Harvest::update() {
//	std::optional<std::pair<int, int>> itemLocation;
//	if (!itemFound) {
//		itemLocation = findClosestTileItem(item->name, villager->xPos, villager->yPos);
//
//		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, x, y);
//
//		x = closestAdj.first;	
//		y = closestAdj.second;
//
//		itemFound = true;
//	}
//
//	if (villager->xPos == x && villager->yPos == y) {
//		mainWorld.objectManager.removeItem(itemLocation->first, itemLocation->second, item);
//		itemFound = false;
//		state = JobState::Completed;
//	}
//}

void HarvestTile::update() {
	Tile& tile = getTileRef(locX, locY);

	auto itemPos = mainWorld.registry.try_get<Position>(item);
	locX = itemPos->x;
	locY = itemPos->y;

	switch (harvestState)
	{
	case HarvestTile::MovingToTile: {
		auto pos = mainWorld.registry.try_get<Position>(villager);
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, locX, locY);

		x = closestAdj.first;
		y = closestAdj.second;

		bool adjacent = isAtTile(pos->x, pos->y, locX, locY);

		if (adjacent) {
			harvestState = HarvestTile::Harvesting;
		}

		break;
	}
	case HarvestTile::Harvesting: {
		harvestClock += Clock::deltaTime;

		int skill = mainWorld.registry.try_get<Skills>(villager)->getSkillLevel(type);
		if (harvestClock > 1.0f * (10.0f / skill)) {

			auto drop = mainWorld.registry.try_get<Harvestable>(item);
			//mainWorld.objectManager.removeItem(locX, locY, item);
			for (int i = 0; i < 1; i++) {
				tile.addObject(locX, locY, drop->produce, true);
				tile.anim.type = animType::NONE;
			}
			tile.removeObject(locX, locY, item);

			tile.markedForHarvest = false;

			state = JobState::Completed;
		}
	}
		break;
	default: 
		break;
	}
	
}

//void HarvestTile::waitingUpdate() {
//
//	if (rule->toolRequired == "None" || rule->toolRequired.empty()) {
//		harvestState = HarvestTile::MovingToTile;
//		state = JobState::Queued;
//		return;
//	}
//
//	for (auto* v : mainWorld.getAllVillagers()) {
//		if (!v) continue;
//		if (v->inventory.has(rule->toolRequired)) {
//			state = JobState::Queued;
//			return;
//		}
//	}
//
//	searchTimer += Clock::deltaTime;
//	if (searchTimer < 3.0f) {
//		return;
//	}
//	searchTimer = 0.0f;
//
//	auto itemLocation = findClosestItemType(locX, locY, 50, [&](const Object& item, int x, int y) {
//		return item.name == rule->toolRequired/* && !item.claimed*/;
//		});
//
//	if (itemLocation) {
//		state = JobState::Queued;
//		std::cout << "Tool found for harvest job, making job available for assignment." << std::endl;
//	}
//}
//
void Build::update() {
	auto pos = mainWorld.registry.try_get<Position>(villager);

	if (!init) {
		staticRecipeEntity = ObjectRegistry::getInstance().getStaticObject(itemName);

		Craftable* c = ObjectRegistry::getInstance().getStaticRegistry().try_get<Craftable>(staticRecipeEntity);

		if (!c) {
			std::cout << "Item not craftable" << std::endl;
			state = JobState::Completed;
			return;
		}

		reserve = findIngredientsForJob(c->ingredients);
		init = true;
		if (reserve.empty()) {
			std::cout << "Ingredients for building not found" << std::endl;
			state = JobState::Completed;
			return;
		}
	}

	if (!grabbedAllItems) {
		auto& itemObj = reserve.front();

		int xPos = itemObj.first.first;
		int yPos = itemObj.first.second;

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, xPos, yPos);
		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos->x, pos->y, xPos, yPos)) {
			if (auto s = mainWorld.atStockpile(xPos, yPos)) {
				//villager->pickUpItem(itemObj.second, xPos, yPos, s);
				getTileRef(xPos, yPos).removeObject(xPos, yPos, itemObj.second);
			}

			reserve.erase(reserve.begin());

			if (reserve.empty()) {
				grabbedAllItems = true;
			}
		}
	}
	else {
		std::pair<int, int> loc = { locX, locY };

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, loc.first, loc.second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos->x, pos->y, loc.first, loc.second)) {

			auto name = ObjectRegistry::getInstance().getStaticRegistry().try_get<Name>(staticRecipeEntity);

			getTileRef(loc.first, loc.second).addObject(loc.first, loc.second, name->name);

			state = JobState::Completed;
		}
	}
}

void BuildFurniture::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);

	switch (jobState)
	{
	case BuildFurniture::Getting: {

		auto closestAdj = findClosestAdjTile(pos.x, pos.y, fX, fY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(pos.x, pos.y, fX, fY)) {
			mainWorld.registry.remove<Position>(item);
			mainWorld.objectManager.removeItem(fX, fY, item);

			auto s = mainWorld.atStockpile(fX, fY);
			if (s) {
				s->removeItem(fX, fY, s->retrieveItem(fX, fY));
			}

			jobState = BuildFurniture::Placing;
		}
		break;
	}

	case BuildFurniture::Placing: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (isAtTile(pos.x, pos.y, tX, tY)) {
			mainWorld.registry.emplace<Position>(item, tX, tY);
			mainWorld.objectManager.addObject(tX, tY, item);
			state = JobState::Completed;

			break;

		}
	}
	default:
		break;
	}
}

//void Refuel::update() {
//
//	auto sharedFuel = fuel.lock();
//	auto sharedTarget = target.lock();
//
//	if (!sharedTarget || (jobState == State::Getting && !sharedFuel)) {
//		state = JobState::Completed;
//		return;
//	}
//	
//	switch (jobState) {
//	case State::Getting: {
//		auto closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, fX, fY);
//		x = closestAdj.first;
//		y = closestAdj.second;
//
//		if (isAtTile(villager->xPos, villager->yPos, fX, fY)) {
//			villager->pickUpItem(fuel.lock(), fX, fY);
//			jobState = State::Fueling;
//		}
//
//		break;
//	}
//
//	case State::Fueling: {
//		auto closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, sharedTarget->x, sharedTarget->y);
//		x = closestAdj.first;
//		y = closestAdj.second;
//
//		if (isAtTile(villager->xPos, villager->yPos, sharedTarget->x, sharedTarget->y)) {
//			villager->inventory.remove(sharedTarget->name);
//			sharedTarget->addFuel(100.0f);
//			sharedTarget->addedFuelJob = false;
//			state = JobState::Completed;
//		}
//
//		break;
//	}
//	}
//
//}
//
void PlaceItem::update() {
	Tile& tile = getTileRef(locX, locY);

	const int vX = mainWorld.registry.get<Position>(villager).x;
	const int vY = mainWorld.registry.get<Position>(villager).y;

	std::pair<int, int> closestAdj = findClosestAdjTile(vX, vY, x, y);

	x = closestAdj.first;
	y = closestAdj.second;

	if (vX == x && vY == y) {
		const std::string itemName = mainWorld.registry.get<Name>(itemToPlace).name;
		tile.addObject(locX, locY, itemName);
		state = JobState::Completed;
	}
}

void Idle::update() {

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& movable = mainWorld.registry.get<Movable>(villager);

	int range = 10;

	if (!initialized) {
		movable.currentSpeed = movable.speed * 5.0f;

		int nx = getRandomInt(pos.x - range, pos.x + range);
		int ny = getRandomInt(pos.y - range, pos.y + range);

		x = nx;
		y = ny;

		waitTime = getRandomFloat(1.0f, 10.0f);
		clock = 0.0f;

		initialized = true;
	}

	if (pos.x == x && pos.y == y) {
		clock += Clock::deltaTime;

		if (clock > waitTime) {
			movable.currentSpeed = movable.speed;
			state = JobState::Completed;
		}
	}
}

void Idle::onInterrupt() {
	state = JobState::Completed;
}

void Plant::update() {

	bool needsSoil = true;

	Tile& tile = getTileRef(locX, locY);

	auto pos = mainWorld.registry.try_get<Position>(villager);

	switch (plantState)
	{
	case Plant::GettingSeed: {
		auto itemLocation = findClosestItemType(pos->x, pos->y, 50, [&](entt::entity item, entt::registry& registry, int x, int y) {
			auto seedComp = registry.try_get<Seed>(item);
			bool claimed = registry.try_get<Claimable>(item)->claimed;
			return seedComp && !claimed;
			});
		if (!itemLocation) {
			std::cout << "No seeds found for planting job, cancelling job." << std::endl;
			state = JobState::Completed;
			return;
		}
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, itemLocation->x, itemLocation->y);
		x = closestAdj.first;
		y = closestAdj.second;
		if (isAtTile(pos->x, pos->y, itemLocation->x, itemLocation->y)) {
			//villager->pickUpItem(itemLocation->item.lock(), itemLocation->x, itemLocation->y);
			tile.removeObject(itemLocation->x, itemLocation->y, itemLocation->item);
			plantState = Plant::Tilling;
		}
		break;
	}
	case Plant::Tilling: {

		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, locX, locY);

		x = closestAdj.first;
		y = closestAdj.second;

		if (tile.type == tileType::SOIL) {
			plantState = Plant::Planting;
		}

		if (pos->x == x && pos->y == y) {
			tillClock += Clock::deltaTime;
			if (tillClock > 2.0f) {
				tile.changeTileType(tileType::SOIL);
				plantClock = 0.0f;
				plantState = Plant::Planting;
			}
		}

		break;

	}
	case Plant::Planting: {
		plantClock += Clock::deltaTime;

		if (plantClock > 3.0f && pos->x == x && pos->y == y) {
			std::cout << "Planting seed at " << locX << ", " << locY << std::endl;
			auto s = ObjectRegistry::getInstance().getStaticObject(seed);

			tile.addObject(locX, locY, seed);
			state = JobState::Completed;
			
		}

		break;
	}
	default:
		break;
	}
}

void Attack::update() {
	attackClock += Clock::deltaTime;

	float range = 10.0f;
	float attackCooldown = 0.05f;
	int dmg = 50;

	if (auto inventory = mainWorld.registry.try_get<Inventory>(villager)) {
		if (auto gun = mainWorld.registry.try_get<Gun>(inventory->itemInHand)) {
			range = gun->range;
			attackCooldown = gun->fire_rate;
			dmg = gun->damage;
		}
	}
	
	if (!mainWorld.registry.valid(target)) {
		state = JobState::Completed;
		if (auto movable = mainWorld.registry.try_get<Movable>(villager)) {
			movable->hasTarget = false;
		}
		return;
	}

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& targetPos = mainWorld.registry.get<Position>(target);

	float dx = static_cast<float>(targetPos.x - pos.x);
	float dy = static_cast<float>(targetPos.y - pos.y);
	float distSq = dx * dx + dy * dy;
	float rangeSq = range * range;

	auto& movable = mainWorld.registry.get<Movable>(villager);

	if (distSq > rangeSq) {
		movable.hasTarget = true;
		x = targetPos.x;
		y = targetPos.y;
	}
	else {
		// Stay in place
		movable.hasTarget = false;
		movable.path.clear();
		x = pos.x;
		y = pos.y;

		if (attackClock > attackCooldown) {
			attackClock -= attackCooldown;
			
			if (auto health = mainWorld.registry.try_get<Health>(target)) {
				health->health -= dmg;
			}

			auto line = bresenham(pos.x, pos.y, targetPos.x, targetPos.y);

			for (int i = 1; i < line.size() - 1; i++) {
				getTileRef(line[i].first, line[i].second).setAnimType(GUN_SHOT);
			}
		}
	}
}

//void Retreat::update() {
//	// Sees if there is cover nearby (if they dont have a weapon, this part isnt important)
//
//	// Sees if there are safe spaces (basically looking for cover for colonists with no weapon)
//
//	// Sees if there are nearby allies with good weapons and health (tune to make colonists not so clingy, also this is a last last last resort)
//
//	// Extra stuff:
//	// Could have them search for a weapon to join the fight
//
//
//	int safeScore = -1;
//
//	if (!villager->threat || villager->threat->dead) {
//		state = JobState::Completed;
//		return;
//
//	}
//
//
//	float dx = villager->xPos - villager->threat->xPos;
//	float dy = villager->yPos - villager->threat->yPos;
//
//	float distance = sqrt(dx * dx + dy * dy);
//	int dim = 64;
//
//	auto threatMap = buildThreatMap(villager->xPos, villager->yPos, dim);
//
//	int startX = villager->xPos - dim / 2;
//	int startY = villager->yPos - dim / 2;
//
//	float bestScore = 99999.0f;
//	std::pair<int, int> bestMove = { 0, 0 };
//
//	std::vector<std::pair<int, int>> dirs = {
//		{1,0}, {0,1}, {-1,0}, {0,-1}
//	};
//
//	std::shuffle(dirs.begin(), dirs.end(), rng);
//
//	bool foundMove = false;
//	int cx = villager->xPos - startX;
//	int cy = villager->yPos - startY;
//
//	for (auto& i : dirs) {
//		int nx = villager->xPos + i.first;
//		int ny = villager->yPos + i.second;
//
//		int fx = nx - startX;
//		int fy = ny - startY;
//
//		if (fx < 0 || fy < 0 || fx >= dim || fy >= dim)
//			continue;
//
//		if (!getTileRef(nx, ny).walkable)
//			continue;
//
//		float score = threatMap[fx][fy];
//
//		if (villager->lastMove != i) {
//			score += 0.5f;
//		}
//
//		score += (villager->xPos + villager->yPos) / 10.0f;
//
//		if (score < bestScore) {
//			bestScore = score;
//			bestMove = i;
//			foundMove = true;
//		}
//	}
//
//	float stayScore = threatMap[cx][cy];
//
//	if (stayScore < bestScore) {
//		bestMove = { 0,0 };
//	}
//
//	if (foundMove && villager->moveClock > villager->speed) {
//		villager->lastTargetX = villager->xPos;
//		villager->lastTargetY = villager->yPos;
//
//		villager->xPos += bestMove.first;
//		villager->yPos += bestMove.second;
//
//		villager->lastMove = bestMove;
//		villager->moveClock = 0.0f;
//	}
//
//	if (!villager->threat || villager->threat->dead) {
//		//std::cout << "Retreated" << std::endl;
//		state = JobState::Completed;
//	}
//}
//


void Sleep::update() {
	mainWorld.registry.get<JobComponent>(villager).activity_state = ActivityState::Sleeping;

	auto& pos = mainWorld.registry.get<Position>(villager);
	x = pos.x;
	y = pos.y;

	Tile& tile = getTileRef(x, y - 1);

	if (pos.x == x && pos.y == y && !sleeping) {
		tile.setAnimType(Z);

		sleeping = true;
		
	} else if (sleeping && mainWorld.dayCycle.getTimePeriod() == TimePeriod::Morning) {
		tile.setAnimType(NONE);

		sleeping = false;
		mainWorld.registry.get<TiredNeed>(villager).tiredness = 0;
		state = JobState::Completed;
	}
}

//std::vector<std::string> getIngredientList(const std::string& itemToCraft);
//

void Craft::update() {
	auto pos = mainWorld.registry.try_get<Position>(villager);
	auto& staticRegistry = ObjectRegistry::getInstance().getStaticRegistry();

	if (!init) {
		staticRecipeEntity = ObjectRegistry::getInstance().getStaticObject(item);

		if (staticRecipeEntity == entt::null) {
			std::cout << "[ERROR] Could not find blueprint recipe for: " << item << std::endl;
			state = JobState::Completed;
			return;
		}

		Craftable* c = staticRegistry.try_get<Craftable>(staticRecipeEntity);

		if (!c) {
			std::cout << "Craftable component not found for item " << item << ", cancelling job." << std::endl;
			state = JobState::Completed;
			return;
		}

		auto& ingredients = c->ingredients;
		reserve = findIngredientsForJob(ingredients);
		init = true;
		if (reserve.empty()) {
			std::cout << "No ingredients found for crafting job, cancelling job." << std::endl;
			state = JobState::Completed;
			return;
		}
	}

	switch (jobState) {
	case (State::FetchingItems): {
		// Find the first ingredient in stockpile
		auto& itemObj = reserve[0];

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, itemObj.first.first, itemObj.first.second);
		x = adjLoc.first;
		y = adjLoc.second;

		if (pos->x == x && pos->y == y) {
			if (auto s = mainWorld.atStockpile(itemObj.first.first, itemObj.first.second)) {
				//mainWorld.objectManager.removeItem(itemObj.first.first, itemObj.first.second, itemObj.second);
				getTileRef(itemObj.first.first, itemObj.first.second).removeObject(itemObj.first.first, itemObj.first.second, itemObj.second);
			}

			reserve.erase(reserve.begin());

			if (reserve.empty()) {
				jobState = State::Crafting;
			}
		}
		break;
	}
	case (State::Crafting): {

		auto* craftableComp = staticRegistry.try_get<Craftable>(staticRecipeEntity);
		if (!craftableComp) {
			state = JobState::Completed;
			return;
		}

		auto loc = findClosestTileItem(craftableComp->benchRequired, pos->x, pos->y);

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, loc->first, loc->second);

		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos->x, pos->y, loc->first, loc->second)) {

			getTileRef(loc->first, loc->second).addObject(loc->first, loc->second, item, true);

			state = JobState::Completed;
		}
		break;
	}
	}
}

//std::vector<std::string> getIngredientList(const std::string& itemToCraft) {
//	std::vector<std::string> result;
//
//	Recipe* recipe = RecipeRegistry::getInstance().get(itemToCraft);
//	if (!recipe) {
//		result.push_back(itemToCraft);
//		return result;
//	}
//	for (auto& [name, count] : recipe->ingredients) {
//		for (int i = 0; i < count; i++) {
//			auto foundItem = mainWorld.findItemInAllStockpile(name);
//			if (!foundItem) {
//				std::vector<std::string> subResult = getIngredientList(name);
//				result.insert(std::end(result), std::begin(subResult), std::end(subResult));
//			}
//			else {
//				result.push_back(name);
//			}
//		}
//	}
//
//	return result;
//}
//
//
//void Move::update() {
//	x = toX;
//	y = toY;
//	if (villager->xPos == toX && villager->yPos == toY) {
//		state = JobState::Completed;
//	}
//}
//
//void MoveItem::update() {
//
//	villager->activity_state = ActivityState::Working;
//
//	if (!itemPickedUp) {
//		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, fromX, fromY);
//		x = closestAdj.first;
//		y = closestAdj.second;
//		if (villager->xPos == x && villager->yPos == y) {
//
//			villager->pickUpItem(itemToMove, fromX, fromY);
//			itemPickedUp = true;
//		}
//	} else {
//
//		std::pair<int, int> closestAdj = findClosestAdjTile(villager->xPos, villager->yPos, toX, toY);
//		x = closestAdj.first;
//		y = closestAdj.second;
//		if (villager->xPos == x && villager->yPos == y) {
//
//			villager->dropItem(itemToMove, toX, toY);
//			itemToMove->claimed = false;
//
//			auto* s = mainWorld.atStockpile(toX, toY);
//			if (s) {
//				s->placeItem(itemToMove, toX, toY);
//			}
//
//
//			state = JobState::Completed;
//		}
//	}
//}

void HaulToStockpile::update() {

	auto pos = mainWorld.registry.try_get<Position>(villager);

	switch (moveState) {


	case (State::PickUpItem): {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, fromX, fromY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (pos->x == x && pos->y == y) {

			mainWorld.objectManager.removeItem(fromX, fromY, itemToMove);

			if (mainWorld.registry.any_of<Position>(itemToMove)) {
				mainWorld.registry.remove<Position>(itemToMove);
			}

			moveState = State::Move;
		}
		break;

	}

	case (State::Move): {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos->x, pos->y, toX, toY);
		x = closestAdj.first;
		y = closestAdj.second;
		if (pos->x == x && pos->y == y) {

			auto* s = mainWorld.atStockpile(toX, toY);
			if (s) {
				mainWorld.registry.emplace_or_replace<Position>(itemToMove, toX, toY);

				auto name = mainWorld.registry.try_get<Name>(itemToMove);

				s->placeItem(itemToMove, toX, toY);
				mainWorld.objectManager.addObject(toX, toY, itemToMove);

				state = JobState::Completed;
			}
			else {
				auto name = mainWorld.registry.try_get<Name>(itemToMove);
				auto spotOpt = mainWorld.findStockpileSpotForItem(name->name, x, y);

				if (spotOpt) {
					auto [stockpile, pos] = *spotOpt;
					stockpile->addItem(itemToMove, pos.first, pos.second);

					toX = pos.first;
					toY = pos.second;

					mainWorld.registry.emplace_or_replace<Position>(itemToMove, toX, toY);
					stockpile->addItem(itemToMove, toX, toY);
				}
				else {
					mainWorld.registry.emplace_or_replace<Position>(itemToMove, pos->x, pos->y);
					mainWorld.addItemToMove(itemToMove, pos->x, pos->y);
					getTileRef(pos->x, pos->y).addObject(pos->x, pos->y, name->name);

					state = JobState::Completed;
				}
			}
		}
		break;
	}

	case (State::Drop):

		break;
	}
}

void FindFood::update() {
	mainWorld.registry.get<JobComponent>(villager).activity_state = ActivityState::Eating;

	auto& pos = mainWorld.registry.get<Position>(villager);

	switch (foodState) {

	case State::Grab: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(pos.x, pos.y, tX, tY)) {
			//getTileRef(tX, tY).removeObject(tX, tY, food);
			mainWorld.registry.remove<Position>(food);
			mainWorld.objectManager.removeItem(tX, tY, food);

			auto s = mainWorld.atStockpile(tX, tY);
			if (s) {
				s->removeItem(tX, tY, s->retrieveItem(tX, tY));
			}
			foodState = State::Find;
		}
		break;
	}


	case State::Find: {
		place = findClosestItemType(pos.x, pos.y, 50, [&](entt::entity item, entt::registry& registry, int x, int y) {
			if (!registry.any_of<Sittable>(item)) return false;
			if (registry.get<Claimable>(item).claimed) return false;

			static const std::pair<int, int> dirs[4] = {
				{0,1},{0,-1},{1,0},{-1,0}
			};

			for (auto& d : dirs) {
				int nx = x + d.first;
				int ny = y + d.second;

				const Tile& t = getTileRef(nx, ny);

				for (auto& item : mainWorld.objectManager.getObjectsAt(nx, ny)) {
					if (registry.any_of<Table>(item)) {
						return true;
					}
				}
			}

			return false;

			});
		if (place.has_value()) {
			mainWorld.registry.get<Claimable>(place->item).claimed = true;
		}
		foodState = State::Eat;
		break;
	}

	case State::Eat: {
		if (place.has_value()) {
			x = place->x;
			y = place->y;
		}
		else {
			x = pos.x;
			y = pos.y;
		}

		if (pos.x == x && pos.y == y) {
			eatTimer += Clock::deltaTime;

			if (eatTimer > 10.0f) {


				if (auto i = mainWorld.registry.try_get<Nutritional>(food)) {
					auto& hunger = mainWorld.registry.get<HungerNeed>(villager).hunger;
					hunger += i->nutrition;

				}
				if (place.has_value()) {
					mainWorld.registry.get<Claimable>(place->item).claimed = false;
				}
				state = JobState::Completed;
			}
		}

		break;
	}
	}
}

//void Sit::update() {
//	x = tX;
//	y = tY;
//
//	clock += Clock::deltaTime;
//
//	if (init && villager->activity_state != ActivityState::Sitting) {
//		state = JobState::Completed;
//		return;
//	}
//
//	if (villager->xPos == tX && villager->yPos == tY) {
//		villager->activity_state = ActivityState::Sitting;
//		init = true;
//	}
//
//	if (clock > villager->tiredness * 0.5f) {
//		clock = 0.0f;
//		state = JobState::Completed;
//		chair.lock()->claimed = false;
//		villager->tiredness = 0;
//		std::cout << "done sitting" << std::endl;
//		return;
//	}
//}
//
//Sit::~Sit() {
//
//	if (villager) {
//		villager->activity_state = ActivityState::None;
//	}
//
//	if (villager && villager->object_in_use) {
//		chair.lock()->claimed = false;
//	}
//}
//
//void Wander::update() {
//
//	if (!hasTarget) {
//		pickNewTarget();
//
//		if (!hasTarget) {
//			std::cout << "no" << std::endl;
//			state = JobState::Completed;
//			return;
//		}
//	}
//
//	x = tX;
//	y = tY;
//
//	if (villager->xPos == tX && villager->yPos == tY) {
//
//		villager->recNeed += 10;
//		state = JobState::Completed;
//	}
//}
//
//void Wander::pickNewTarget() {
//
//	int range = 6;
//
//	for (int i = 0; i < 10; i++) {
//
//		int nx = villager->xPos + getRandomInt(-range, range);
//		int ny = villager->yPos + getRandomInt(-range, range);
//
//		if (!getTileRef(nx, ny).walkable)
//			continue;
//
//		tX = nx;
//		tY = ny;
//
//		hasTarget = true;
//
//		return;
//	}
//
//	hasTarget = false;
//}
//
//void Meditate::update() {
//	if (!hasTarget) {
//		x = villager->xPos;
//		y = villager->yPos;
//		hasTarget = true;
//	}
//	if (villager->xPos == x && villager->yPos == y) {
//		if (villager->clock > 15.0f) {
//			villager->tiredness = std::max(0, villager->tiredness - 20);
//			state = JobState::Completed;
//		}
//	}
//}
//
//void Talk::update() {
//	if (!other) {
//		state = JobState::Completed;
//		return;
//	}
//
//	switch (talkState) {
//	case (State::WalkTo): {
//		int dx = std::abs(villager->xPos - other->xPos);
//		int dy = std::abs(villager->yPos - other->yPos);
//
//		auto adj = findClosestAdjTile(villager->xPos, villager->yPos, other->xPos, other->yPos);
//		x = adj.first;
//		y = adj.second;
//
//		Tile& tile = getTileRef(villager->xPos, villager->yPos - 1);
//
//		if (dx + dy <= 3) {
//			// Stop moving when in range
//			x = villager->xPos;
//			y = villager->yPos;
//
//			talkState = State::TalkTo;
//		}
//
//		break;
//	}
//	case (State::TalkTo): {
//		Tile& tile = getTileRef(villager->xPos, villager->yPos - 1);
//		//tile.setAnimType(SPEECH_BUBBLE);
//
//		clock += Clock::deltaTime;
//
//		if (other->activity_state != ActivityState::Socializing) {
//			Tile& otherTile = getTileRef(other->xPos, other->yPos);
//			otherTile.setAnimType(NONE);
//			tile.setAnimType(NONE);
//
//			villager->nearby = nullptr;
//			villager->social = 100;
//			state = JobState::Completed;
//		}
//
//		if (clock > 10.0f) {
//			clock = 0.0f;
//			villager->social = 100;
//
//			villager->nearby = nullptr;
//
//			tile.setAnimType(NONE);
//
//			villager->log(villager->firstname + " " + villager->lastname + " talked to " + other->firstname + " " + other->lastname);
//			state = JobState::Completed;
//		}
//
//		break;
//	}
//	}
//}

std::vector<std::pair<std::pair<int, int>, entt::entity>>
findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients) {

	std::vector<std::pair<std::pair<int, int>, entt::entity>> result;
	std::vector<entt::entity> reserved;
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

			if (auto claimed = mainWorld.registry.try_get<Claimable>(itemLocation->second)) {
				claimed->claimed = true;
			}

			reserved.push_back(itemLocation->second);
			result.push_back(*itemLocation);
		}
		if (!success) {
			break;
		}
	}

	if (!success) {
		for (entt::entity obj : reserved) {
			auto claimed = mainWorld.registry.try_get<Claimable>(obj);
			claimed->claimed = false;
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