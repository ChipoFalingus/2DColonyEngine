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
std::vector<ItemLocation> findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients);
void removeBlueprint(int x, int y);


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

		for (auto& job : JobList) {
			if (!job)
				continue;

			if (job->state != JobState::Queued)
				continue;

			if (job_component.panicClock < job_component.panicDuration)
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
			evaluateJobDanger(job);
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

			JobList.erase(JobList.begin() + i);
		}
		else {
			i++;
		}
	}
}

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

		int xPos = itemObj.x;
		int yPos = itemObj.y;

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, xPos, yPos);
		x = adjLoc.first;
		y = adjLoc.second;

		if (isAtTile(pos->x, pos->y, xPos, yPos)) {
			getTileRef(xPos, yPos).removeObject(xPos, yPos, itemObj.item);
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

			removeBlueprint(loc.first, loc.second);
			getTileRef(loc.first, loc.second).addObject(loc.first, loc.second, itemName);
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
				s->removeItem(fX, fY, item);
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
			mainWorld.registry.get<Claimable>(item).claimed = false;
			mainWorld.objectManager.addObject(tX, tY, item);
			state = JobState::Completed;

			break;

		}
	}
	default:
		break;
	}
}

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
		//movable.currentSpeed = movable.speed * 5.0f;

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

	auto& attackComponent = mainWorld.registry.get<CombatComponent>(villager);

	float range;
	float attackCooldown;
	int dmg;

	if (attackComponent.equippedWeapon != entt::null) {
		if (auto gun = mainWorld.registry.try_get<Gun>(attackComponent.equippedWeapon)) {
			range = gun->range;
			attackCooldown = gun->fire_rate;
			dmg = gun->damage;
		}
	}

	auto& pos = mainWorld.registry.get<Position>(villager);
	
	if (!mainWorld.registry.valid(target)) {
		auto newThreat = findClosestItemType(pos.x, pos.y, 25, [&](entt::entity entity, entt::registry& reg, int x, int y) {
			return reg.try_get<Hostile>(entity) && (entity != villager);
			});


		if (newThreat.has_value()) {
			target = newThreat.value().item;
		}
		else {
			if (auto movable = mainWorld.registry.try_get<Movable>(villager)) {
				movable->hasTarget = false;
			}
			state = JobState::Completed;
		}

		return;
	}

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

void Retreat::update() {
	auto& pos = mainWorld.registry.get<Position>(villager);
	
	if (!init) {
		auto& jobComponent = mainWorld.registry.get<JobComponent>(villager);
		jobComponent.panicClock = 0.0f;
		jobComponent.panicDuration = 10.0f;

		jobComponent.clearInterruptedJobs();

		init = true;
	}

	if (!foundPath) {
		auto& combatComponent = mainWorld.registry.get<CombatComponent>(villager);

		int dim = 64;
		int half = dim / 2;
		std::vector<std::vector<float>> threatMap = buildThreatMap(pos.x, pos.y, dim);

		float bestScore = std::numeric_limits<float>::max();
		std::pair<int, int> bestLocation = { pos.x, pos.y };

		for (int i = -half; i < half; i++) {
			for (int j = -half; j < half; j++) {
				int worldX = pos.x + i;
				int worldY = pos.y + j;

				int localX = i + half;
				int localY = j + half;

				int dx = std::abs(worldX - pos.x);
				int dy = std::abs(worldY - pos.y);

				auto& targetPos = mainWorld.registry.get<Position>(threat);
				int distToThreat = std::abs(targetPos.x - worldX) + std::abs(targetPos.y - worldY);

				int distFromSelf = std::abs(i) + std::abs(j);

				float score = threatMap[localX][localY];
				score -= distToThreat * 0.5f;  
				score += distFromSelf * 0.05f;

				if (score < bestScore) {
					bestScore = score;
					bestLocation = { worldX, worldY };
				}
			}
		}

		x = bestLocation.first;
		y = bestLocation.second;

		foundPath = true;
	}

	if (pos.x == x && pos.y == y) {
		foundPath = false;
	}

	auto closestThreat = findClosestItemType(pos.x, pos.y, 50, [&](entt::entity entity, entt::registry& reg, int x, int y) {
		return reg.try_get<Hostile>(entity) && (entity != villager);
		});

	if (!closestThreat.has_value()) {
		state = JobState::Completed;
	}
}

void Sleep::update() {
	mainWorld.registry.get<JobComponent>(villager).activity_state = ActivityState::Sleeping;
	auto& tiredComponent = mainWorld.registry.get<TiredNeed>(villager);

	auto& pos = mainWorld.registry.get<Position>(villager);
	if (tiredComponent.bedLocation.has_value()) {
		x = tiredComponent.bedLocation->first;
		y = tiredComponent.bedLocation->second;
	}
	else {
		x = pos.x;
		y = pos.y;
	}

	Tile& tile = getTileRef(x, y - 1);

	if (pos.x == x && pos.y == y && !sleeping) {
		tile.setAnimType(Z);

		sleeping = true;
		
	} else if (sleeping && mainWorld.dayCycle.getTimePeriod() == TimePeriod::Morning) {
		tile.setAnimType(NONE);

		sleeping = false;
		tiredComponent.tiredness = 0;
		state = JobState::Completed;
	}
}

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

		auto adjLoc = findClosestAdjTile(pos->x, pos->y, itemObj.x, itemObj.y);
		x = adjLoc.first;
		y = adjLoc.second;

		if (pos->x == x && pos->y == y) {
			if (auto s = mainWorld.atStockpile(itemObj.x, itemObj.y)) {
				getTileRef(itemObj.x, itemObj.y).removeObject(itemObj.x, itemObj.y, itemObj.item);
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

	mainWorld.registry.get<Claimable>(food).claimed = true;
	mainWorld.registry.get<JobComponent>(villager).activity_state = ActivityState::Eating;

	auto& pos = mainWorld.registry.get<Position>(villager);

	switch (foodState) {

	case State::Grab: {
		std::pair<int, int> closestAdj = findClosestAdjTile(pos.x, pos.y, tX, tY);
		x = closestAdj.first;
		y = closestAdj.second;

		if (isAtTile(pos.x, pos.y, tX, tY)) {
			mainWorld.registry.remove<Position>(food);
			mainWorld.objectManager.removeItem(tX, tY, food);

			auto s = mainWorld.atStockpile(tX, tY);
			if (s) {
				s->removeItem(tX, tY, food);
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
			auto& pos = mainWorld.registry.get<Position>(place->item);
			std::cout << "Found Table + Chair at " << pos.x << " " << pos.y << std::endl;
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

std::vector<ItemLocation> findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients) {

	std::vector<ItemLocation> result;
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

			if (auto claimed = mainWorld.registry.try_get<Claimable>(itemLocation->item)) {
				claimed->claimed = true;
			}

			reserved.push_back(itemLocation->item);
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


void Talk::update() {

	if (!mainWorld.registry.valid(other)) {
		state = JobState::Completed;
		return;
	}
	auto& jobComponent = mainWorld.registry.get<JobComponent>(villager);
	auto& otherJobComponent = mainWorld.registry.get<JobComponent>(other);

	jobComponent.activity_state = ActivityState::Socializing;
	otherJobComponent.activity_state = ActivityState::Socializing;

	if (!dynamic_cast<Talk*>(otherJobComponent.currentJob)) {
		state = JobState::Completed;
		return;
	}

	auto& pos = mainWorld.registry.get<Position>(villager);
	auto& otherPos = mainWorld.registry.get<Position>(other);

	switch (talkState) {
	case (State::WalkTo): {
		int dx = std::abs(pos.x - otherPos.x);
		int dy = std::abs(pos.y - otherPos.y);

		if (dx + dy <= 4) {
			x = pos.x;
			y = pos.y;

			talkState = State::TalkTo;
		}
		else {
			auto adj = findClosestAdjTile(pos.x, pos.y, otherPos.x, otherPos.y);
			x = adj.first;
			y = adj.second;
		}

		break;
	}
	case (State::TalkTo): {
		Tile& tile = getTileRef(pos.x, pos.y - 1);
		Tile& otherTile = getTileRef(otherPos.x, otherPos.y - 1);
		tile.setAnimType(SPEECH_BUBBLE);

		clock += Clock::deltaTime;

		auto& socialNeed = mainWorld.registry.get<Social>(villager);
		auto& otherSocialNeed = mainWorld.registry.get<Social>(other);

		if (!dynamic_cast<Talk*>(otherJobComponent.currentJob)) {
			otherTile.setAnimType(NONE);
			tile.setAnimType(NONE);

			socialNeed.social = 100;
			state = JobState::Completed;
		}

		if (clock > 10.0f) {
			clock = 0.0f;
			socialNeed.social = 100;
			otherSocialNeed.social = 100;

			otherTile.setAnimType(NONE);
			tile.setAnimType(NONE);

			otherJobComponent.currentJob->state = JobState::Completed;
			state = JobState::Completed;
		}

		break;
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

void removeBlueprint(int x, int y) {
	Tile& tile = getTileRef(x, y);
	for (auto& item : mainWorld.objectManager.getObjectsAt(x, y)) {
		if (mainWorld.registry.any_of<BlueprintTag>(item)) {
			mainWorld.registry.remove<Position>(item);
			mainWorld.objectManager.removeItem(x, y, item);
			tile.removeObject(x, y, item);
			std::cout << "Removed blueprint at " << x << ", " << y << std::endl;
			break;
		}
	}
}

void evaluateJobDanger(Job* job) {
	auto closestThreat = findClosestItemType(job->x, job->y, 50, [&](entt::entity entity, entt::registry& reg, int x, int y) {
		return reg.try_get<Hostile>(entity) && (entity != job->villager);
		});
	if (closestThreat.has_value()) {
		job->state = JobState::Waiting;
	}
	else {
		job->state = JobState::Queued;
	}
}