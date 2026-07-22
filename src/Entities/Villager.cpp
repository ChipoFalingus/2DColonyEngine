#include <random>

#include <entt/entt.hpp>

#include "CreatureComponents.h"
#include "ItemComponents.h"
#include "Utility/ItemUtils.h"
#include "World/World.h"

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
	"Tom",
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


};

std::vector<std::string> lastnames = {
    "Hoff",
    "Gordon",
    "Chiu",
    "Nahmias",
    "Kim",
    "Hart",
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
	"Robinson",
	"Walker",
	"Young",
	"Allen",
	"King",
	"Wright",
	"Scott",
	"Torres",

	"Coomer",

};

entt::entity spawnVillager(int x, int y) {
	auto& registry = mainWorld.registry;
	auto entity = registry.create();

	registry.emplace<Villager>(entity);

	registry.emplace<Position>(entity, x, y);

	int r = getRandomInt(100, 255);
	int g = getRandomInt(100, 255);
	int b = getRandomInt(100, 255);

	registry.emplace<Renderable>(entity, L'☺', glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f));

	std::string name = names[getRandomInt(0, names.size() - 1)] + " " + lastnames[getRandomInt(0, lastnames.size() - 1)];

	float moveSpeed = 0.1f;
	float clock = 0.0f;
	int initTargetX = x;
	int initTargetY = y;

	registry.emplace<Name>(entity, name);
	registry.emplace<Movable>(entity, moveSpeed, moveSpeed, clock, initTargetX, initTargetY, true);
	registry.emplace<Health>(entity, 100);
	registry.emplace<HungerNeed>(entity, 100);
	registry.emplace<TiredNeed>(entity, 0);
	registry.emplace<TemperatureNeed>(entity, getRandomFloat(60.0f, 80.0f));

	registry.emplace<JobComponent>(entity, nullptr);
		
	auto skillList = getAllSkillTypes();
	int rand = getRandomInt(0, skillList.size() - 1); 

	Skills villagerSkills;

	for (int i = 0; i < skillList.size(); i++) {
		if (i == rand) {
			// One random high skill
			villagerSkills.setSkillLevel(skillList[i], getRandomInt(10, 13));
		}
		else {
			villagerSkills.setSkillLevel(skillList[i], getRandomInt(1, 3));
		}
	}

	registry.emplace<Skills>(entity, villagerSkills);

	return entity;
}


void VillagerSystem(float deltaTime) {
	auto& registry = mainWorld.registry;
	auto view = registry.view<Position, Movable, Health>();
	
	std::vector<entt::entity> deadEntities;

	for (auto [entity, pos, movable, health] : view.each()) {
		if (health.health <= 0) {
			deadEntities.push_back(entity);
		}
	}

	for (auto entity : deadEntities) {
		registry.destroy(entity);
	}

	updateHunger();
	updateTiredness();
	updateWork();
}

void updateTiredness() {
	auto& registry = mainWorld.registry;
	auto view = registry.view<TiredNeed, JobComponent>();
	for (auto [entity, tiredness, jobComponent] : view.each()) {
		tiredness.clock += Clock::deltaTime;
		if (tiredness.clock >= 10.0f) {
			tiredness.clock = 0.0f;
			tiredness.tiredness += 1;
			if (tiredness.tiredness > 100) {
				tiredness.tiredness = 100;
			}
		}

		if (tiredness.tiredness <= 20) continue;
		if (mainWorld.dayCycle.getTimePeriod() != TimePeriod::Night) continue;
		float score = std::pow(tiredness.tiredness * 0.01f, 3) * 100.0f;

		if (mainWorld.dayCycle.getTimePeriod() == TimePeriod::Night) {
			score *= 4.0f;
		}

		Job* job = new Sleep(entity, entt::null, SkillType::None);
		job->priority = score;

		jobComponent.proposeJob(job);
	}
}

void updateHunger() {
	auto& registry = mainWorld.registry;
	auto view = registry.view<HungerNeed, JobComponent>();
	for (auto [entity, hunger, jobComponent] : view.each()) {
		hunger.clock += Clock::deltaTime;
		if (hunger.clock >= 1.0f) {
			hunger.clock = 0.0f;
			hunger.hunger -= 1;
			if (hunger.hunger < 0) {
				hunger.hunger = 0;
			}
		}

		hunger.findFoodClock += Clock::deltaTime;

		if (hunger.findFoodClock < 2.0f) continue;
		hunger.findFoodClock = 0.0f;

		if (jobComponent.currentJob && dynamic_cast<FindFood*>(jobComponent.currentJob)) {
			continue;
		}

		if (hunger.hunger >= 80) continue;
		auto& pos = mainWorld.registry.get<Position>(entity);

		auto foodLocation = findClosestItemType(pos.x, pos.y, 50, [](entt::entity entity, entt::registry& reg, int x, int y) {
			auto* food = reg.try_get<Nutritional>(entity);
			auto* claim = reg.try_get<Claimable>(entity);

			return food && (claim ? !claim->claimed : true);
			});

		if (foodLocation.has_value()) {
			float score = std::pow(hunger.hunger * 0.01f, 2) * 100.0f;

			Job* job = new FindFood(entity, entt::null, SkillType::None, foodLocation->x, foodLocation->y, foodLocation->item);
			auto& c = registry.get<Claimable>(foodLocation->item).claimed = true;
			
			job->priority = score;

			jobComponent.proposeJob(job);
		}
	}
}

void updateTempNeed() {
	auto& registry = mainWorld.registry;
	auto view = registry.view<JobComponent, Position, TemperatureNeed>();
	for (auto [entity, work, pos, tempNeed] : view.each()) {

		tempNeed.clock += Clock::deltaTime;
		if (tempNeed.clock < 5.0f) continue;
		tempNeed.clock = 0.0f;

		float deviation = bellCurve(mainWorld.getTemperatureMapIndex(pos.x, pos.y), tempNeed.preferredTemp, 8);
		float score = deviation * 10.0f;

		if (work.activity_state != ActivityState::None) {
			score *= 0.5f;
		}

		if (0.5f > deviation) {
			//return { UtilityType::WARMING_UP, score};
		}
		else {
			continue;
		}
	}
}

void updateWork() {
	
	auto& registry = mainWorld.registry;
	auto view = registry.view<JobComponent, Movable, HungerNeed>();
	for (auto [entity, work, movable, hunger] : view.each()) {
		if (!work.interrupted.empty()) {
			std::sort(work.interrupted.begin(), work.interrupted.end(), [](const Job* a, const Job* b) {
				return a->priority > b->priority;
				});

			if (!work.currentJob || work.interrupted[0]->priority > work.currentJob->priority) {
				Job* higherPriorityJob = work.interrupted.front();
				work.interrupted.erase(work.interrupted.begin());

				if (work.currentJob) {
					work.interrupted.push_back(work.currentJob);
				}

				work.currentJob = higherPriorityJob;
				movable.hasTarget = false;
			}
		}

		if (!work.currentJob) {
			/*std::cout << "Idle job assigned" << std::endl;
			Job* job = new Idle(entity, entt::null, SkillType::None);
			job->priority = 10;
			work.proposeJob(job);*/
			continue;
		}

		work.currentJob->update();

		if (work.currentJob->state == JobState::Completed) {
			std::cout << "Job completed for villager, deleting job" << std::endl;
			delete work.currentJob;
			work.currentJob = nullptr;
			movable.hasTarget = false;
			continue;
		}

		if (!movable.hasTarget || movable.targetX != work.currentJob->x || movable.targetY != work.currentJob->y) {
			movable.targetX = work.currentJob->x;
			movable.targetY = work.currentJob->y;
			movable.hasTarget = true;
		}
	}
}


std::string activityStateToString(ActivityState state) {
	switch (state) {

	case ActivityState::None:
		return "Nothing";
		break;
	case ActivityState::Sitting:
		return "Sitting";
		break;
	case ActivityState::Sleeping:
		return "Sleeping";
		break;
	case ActivityState::Eating:
		return "Eating";
		break;
	case ActivityState::Wandering:
		return "Wandering";
		break;
	case ActivityState::Working:
		return "Working";
		break;
	case ActivityState::Meditating:
		return "Meditating";
		break;
	case ActivityState::Socializing:
		return "Socializing";
		break;
	case ActivityState::Retreating:
		return "Retreating";
		break;
	case ActivityState::Attacking:
		return "Attacking";
		break;
	}

	return "NULL";
}

//void Villager::sense() {
//	if (checkThreatsClock > 1.0f) {
//		checkThreatsClock = 0.0f;
//		auto z = findClosestCreatureType<Zombie>(xPos, yPos, alertness);
//		if (z) {
//			threat = z;
//		}
//		else {
//			threat = nullptr;
//		}
//	}
//}
//
//void Villager::idle() {
//
//}
//
//Evaluation Villager::evaluateEating() {
//	if (findFoodClock < 2.0f) return { UtilityType::EAT, -1.0f };
//	findFoodClock = 0.0f;
//	if (hunger >= 80) return { UtilityType::EAT, -1.0f };
//
//	auto foodLocation = findClosestItemType(xPos, yPos, 50, [](const Object& obj, int x, int y) {
//		return obj.type == Type::Food && !obj.claimed;
//		});
//
//	if (foodLocation.has_value()) {
//		if (auto lockedFood = foodLocation->item.lock()) {
//			float weight = traits[TraitType::EatWeight] + 0.5f;
//			float score = std::pow(hunger * 0.01f, 2) * 100.0f * weight;
//			return { UtilityType::EAT, score, lockedFood, foodLocation->x, foodLocation->y};
//		}
//	}
//	return { UtilityType::EAT, -1.0f };
//}
//
//Evaluation Villager::evaluateCombat() {
//	if (!threat) return { UtilityType::IDLE, 0.0f };
//
//	if (!itemInHand) {
//		return { UtilityType::RETREAT, 9999.0f };
//	}
//	return { UtilityType::ATTACK, 9990.0f };
//}
//
//Evaluation Villager::evaluateMeditation() {
//	return { UtilityType::MEDITATE, social * 1.0f };
//}
//
//Evaluation Villager::evaluateSocializing() {
//	if (social > 75) return { UtilityType::SOCIALIZE, -1.0f };
//
//	Villager* bestTarget = nullptr;
//	float closestDist = 9999.0f;
//
//	for (auto& other : mainWorld.getAllVillagers()) {
//		if (other == this) continue;
//		if (other->activity_state == ActivityState::Sleeping) continue;
//
//		float dist = getDistance(other);
//		if (dist < 15.0f && dist < closestDist) {
//			closestDist = dist;
//			bestTarget = other;
//		}
//	}
//
//	if (!bestTarget) {
//		return { UtilityType::SOCIALIZE, -1.0f };
//	}
//
//	this->nearby = bestTarget;
//
//	float socialFactor = social * 0.01f;
//	float tirednessFactor = 1 - (tiredness * 0.01f);
//	float score = std::pow(1 - socialFactor, 3) * 100.0f;
//
//	return { UtilityType::SOCIALIZE, score * tirednessFactor };
//}
//
//Evaluation Villager::evaluateSitting() {
//	if (tiredness <= 20) return { UtilityType::SIT, -1.0f };
//	if (findChairClock < 5.0f) return { UtilityType::SIT, -1.0f };
//	findChairClock = 0.0f;
//
//	auto loc = findClosestItemType(xPos, yPos, 30, [](const Object& obj, int x, int y) {
//		return (obj.name == "Wooden Chair" || obj.name == "Stone Chair") && !obj.claimed
//			&& !mainWorld.atStockpile(x, y);
//		});
//
//	if (loc) {
//		if (mainWorld.dayCycle.getTimePeriod() == TimePeriod::Night) {
//			return { UtilityType::SIT, -1.0f };
//		}
//
//		float weight = 2.0f * traits[TraitType::TiredWeight] + 0.5f;
//		float score = std::pow(tiredness * 0.01f, 3) * 100.0f * weight;
//		return { UtilityType::SIT, score, loc.value().item.lock(), loc.value().x, loc.value().y};
//	}
//	else {
//		return { UtilityType::SIT, -1.0f};
//	}
//}
//
//Evaluation Villager::evaluateSleeping() {
//    if (tiredness <= 20) return { UtilityType::SLEEP, 0.0f };
//
//	if (mainWorld.dayCycle.getTimePeriod() != TimePeriod::Night) {
//		return { UtilityType::SLEEP, -1.0f };
//	}
//
//	float weight = traits[TraitType::TiredWeight] + 0.5f;
//	float score = std::pow(tiredness * 0.01f, 3) * 100.0f * weight;
//
//	if (mainWorld.dayCycle.getTimePeriod() == TimePeriod::Night) {
//		score *= 4.0f;
//	}
//
//	return { UtilityType::SLEEP, score };
//}
//
//Evaluation Villager::evaluateWarmingUp() {
//	if (heatClock < 5.0f) return { UtilityType::WARMING_UP, -1.0f };
//	heatClock = 0.0f;
//
//	float deviation = bellCurve(mainWorld.getTemperatureMapIndex(xPos, yPos), preferredTemp, 8);
//	float score = deviation * 10.0f * (traits.at(TraitType::TemperatureToleranceWeight) + 0.5f);
//
//	if (activity_state != ActivityState::None) {
//		score *= 0.5f;
//	}
//
//	if (0.5f > deviation) {
//		return { UtilityType::WARMING_UP, score};
//	}
//	else {
//		return { UtilityType::WARMING_UP, -1.0f};
//	}
//	
//}
//
//Evaluation Villager::evaluateIdle() {
//	return { UtilityType::IDLE, (tiredness * 0.25f) + 10.0f };
//}
//
//Evaluation Villager::evaluateWandering() {
//
//	float recWeight = (100.0f - recNeed) * 0.1f;
//	float traitWeight = (1.0f - traits[TraitType::SocialWeight]) * 5.0f;
//
//	return { UtilityType::WANDER, recWeight * traitWeight };
//}
//
//
//void Villager::decide() {
//	if (tirednessClock > 10.0f && !sleeping) { tiredness++; tirednessClock = 0.0f; }
//	if (hungerClock > 10.0f) { hunger--; hungerClock = 0.0f; }
//	if (socialClock > 12.0f * traits.at(TraitType::SocialWeight) + 1.0f) { social--; socialClock = 0.0f; }
//	if (recreationClock > 12.0f * traits.at(TraitType::WorkWeight) + 1.0f) { recNeed--; recreationClock = 0.0f; }
//
//	
//	std::vector<Evaluation> options = {
//		evaluateCombat(),
//		evaluateSleeping(),
//		evaluateEating(),
//		evaluateWarmingUp(),
//		evaluateSocializing(),
//		evaluateSitting(),
//		//evaluateWandering(),
//		//evaluateIdle(),
//		//evaluateMeditation(),
//	};
//
//	if (currentJob) {
//		options.push_back({ UtilityType::CURRENT, (float)currentJob->priority });
//	}
//
//	std::sort(interrupted.begin(), interrupted.end(), [](const Job* a, const Job* b) {
//		return a->priority > b->priority;
//		});
//
//	if (!interrupted.empty()) {
//		options.push_back({UtilityType::INTERRUPTED_RESUME, (float)interrupted[0]->priority});
//	}
//
//	std::sort(options.begin(), options.end(), [](const Evaluation& a, const Evaluation& b) {
//		return a.score > b.score;
//		});
//
//	Evaluation best = options.front();
//
//	if (currentJob) {
//		if (best.type == UtilityType::CURRENT) {
//			currentJob->state = JobState::Active;
//			return;
//		}
//
//		if (best.type == UtilityType::ATTACK && activity_state == ActivityState::Attacking) { currentJob->state = JobState::Active; return; }
//		if (best.type == UtilityType::RETREAT && activity_state == ActivityState::Retreating) { currentJob->state = JobState::Active; return; }
//		if (best.type == UtilityType::SLEEP && activity_state == ActivityState::Sleeping) { currentJob->state = JobState::Active; return; }
//		if (best.type == UtilityType::SOCIALIZE && activity_state == ActivityState::Socializing) { currentJob->state = JobState::Active; return; }
//		if (best.type == UtilityType::EAT && activity_state == ActivityState::Eating) { currentJob->state = JobState::Active; return; }
//		if (best.type == UtilityType::SIT && activity_state == ActivityState::Sitting) { currentJob->state = JobState::Active; return; }
//		if (best.type == UtilityType::WANDER && activity_state == ActivityState::Wandering) { currentJob->state = JobState::Active; return; }
//
//		if (best.score > (float)currentJob->priority) {
//			interrupted.push_back(currentJob);
//			currentJob = nullptr;
//		}
//		else {
//			currentJob->state = JobState::Active;
//			return;
//		}
//	}
//	Job* job = nullptr;
//	switch (best.type) {
//	case UtilityType::ATTACK:
//		activity_state = ActivityState::Attacking;
//		job = new Attack(this, nullptr, SkillType::None, threat);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//
//	case UtilityType::RETREAT:
//		currentPath.clear();
//		activity_state = ActivityState::Retreating;
//		job = new Retreat(this, nullptr, SkillType::None, threat);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//
//	case UtilityType::SLEEP:
//		activity_state = ActivityState::Sleeping;
//		job = new Sleep(this, nullptr, SkillType::None);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//
//	case UtilityType::EAT: {
//		best.targetItem->claimed = true;
//		activity_state = ActivityState::Eating;
//
//		auto f = static_pointer_cast<Food>(best.targetItem);
//		job = new FindFood(this, nullptr, SkillType::None, best.targetX, best.targetY, f);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//	}
//	case UtilityType::WARMING_UP: 
//		currentPath = findPath(xPos, yPos, findBestTemperatureTile(xPos, yPos, 25, preferredTemp));
//		break;
//
//	case UtilityType::MEDITATE:
//		activity_state = ActivityState::Meditating;
//		job = new Meditate(this, nullptr, SkillType::None);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//
//	case UtilityType::SIT:
//		activity_state = ActivityState::Sitting;
//		best.targetItem->claimed = true;
//		job = new Sit(this, nullptr, SkillType::None, best.targetItem, best.targetX, best.targetY);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//	case UtilityType::WANDER:
//		activity_state = ActivityState::Wandering;
//		job = new Wander(this, nullptr, SkillType::None);
//		job->priority = best.score;
//		currentJob = job;
//		break;
//	case UtilityType::SOCIALIZE: {
//		if (nearby) {
//
//			if (nearby->currentJob && nearby->getCurrentJob()->priority > best.score) {
//				currentJob = nullptr;
//				break;
//			}
//
//			if (nearby->currentJob) {
//				nearby->interrupted.push_back(nearby->currentJob);
//			}
//
//			nearby->activity_state = ActivityState::Socializing;
//
//			Job* partnerJob = new Talk(nearby, nullptr, SkillType::None, this);
//			partnerJob->priority = best.score;
//
//			nearby->currentJob = partnerJob;
//
//			activity_state = ActivityState::Socializing;
//
//			job = new Talk(this, nullptr, SkillType::None, nearby);
//			job->priority = best.score;
//			currentJob = job;
//		}
//
//		break;
//	}
//
//	case UtilityType::INTERRUPTED_RESUME:
//		currentJob = interrupted.front();
//		interrupted.erase(interrupted.begin());
//
//		if (currentJob) {
//			if (dynamic_cast<Attack*>(currentJob))   activity_state = ActivityState::Attacking;
//			else if (dynamic_cast<Retreat*>(currentJob)) activity_state = ActivityState::Retreating;
//			else if (dynamic_cast<Sleep*>(currentJob))   activity_state = ActivityState::Sleeping;
//			else if (dynamic_cast<FindFood*>(currentJob)) activity_state = ActivityState::Eating;
//		}
//
//		if (best.type == UtilityType::INTERRUPTED_RESUME) {
//			if (best.score <= (float)currentJob->priority) {
//				currentJob->state = JobState::Active;
//				return;
//			}
//		}
//
//		if (best.score > (float)currentJob->priority) {
//			interrupted.push_back(currentJob);
//			currentJob = nullptr;
//		}
//		else {
//			currentJob->state = JobState::Active;
//			return;
//		}
//
//		break;
//	default:
//		activity_state = ActivityState::None;
//		break;
//	}
//
//	if (currentJob) {
//		currentJob->state = JobState::Active;
//	}
//}
//
//void Villager::move() {
//
//	if (currentJob) {
//		currentJob->update();
//
//		if (currentPath.empty() || currentJob->x != lastTargetX || currentJob->y != lastTargetY) {
//			currentPath = findPath(xPos, yPos, { currentJob->x, currentJob->y });
//			lastTargetX = currentJob->x;
//			lastTargetY = currentJob->y;
//		}
//
//		if (currentJob->state == JobState::Completed) {
//			currentJob = nullptr;
//		}
//	}
//
//	if (!currentPath.empty()) {
//		if (moveClock > speed) {
//			auto nextStep = currentPath.front();
//			currentPath.erase(currentPath.begin());
//			if (!getTileRef(nextStep.first, nextStep.second).walkable) {
//				currentPath = findPath(xPos, yPos, { currentJob->x, currentJob->y });
//				return;
//			}
//			else {
//				xPos = nextStep.first;
//				yPos = nextStep.second;
//			}
//			moveClock = 0.0f;
//		}
//	}
//}
//
//void Villager::doWork() {
//
//	clock += Clock::deltaTime;
//	moveClock += Clock::deltaTime;
//	idleClock += Clock::deltaTime;
//	findFoodClock += Clock::deltaTime;
//	checkThreatsClock += Clock::deltaTime;
//	hungerClock += Clock::deltaTime;
//	findBedClock += Clock::deltaTime;
//	tirednessClock += Clock::deltaTime;
//	socialClock += Clock::deltaTime;
//	recreationClock += Clock::deltaTime;
//
//	heatClock += Clock::deltaTime;
//	findChairClock += Clock::deltaTime;
//
//	if (toolInHand) {
//		harvestTime = toolInHand->efficiency / materialToEfficiency(toolInHand->material);
//	}
//	else {
//		harvestTime = 1.0f;
//	}
//
//	sense();
//	decide();
//	move();
//}
//
//void Villager::pickUpItem(std::shared_ptr<Object> item, int x, int y, Stockpile* stockpile) {
//	mainWorld.objectManager.removeItem(x, y, item);
//	inventory.add(item);
//}
//
//void Villager::dropItem(std::shared_ptr<Object> item, int x, int y) {
//	if (!inventory.has(item->name)) return;
//	inventory.remove(item->name);
//	mainWorld.objectManager.addObject(x, y, item);
//}
//
//
//std::string getJobName(const Job* job) {
//	if (!job) return "None/Null";
//
//	if (dynamic_cast<const Attack*>(job))    return "Attack";
//	if (dynamic_cast<const Retreat*>(job))   return "Retreat";
//	if (dynamic_cast<const Sleep*>(job))     return "Sleep";
//	if (dynamic_cast<const FindFood*>(job))  return "FindFood";
//	if (dynamic_cast<const Talk*>(job))      return "Talking";
//	if (dynamic_cast<const Sit*>(job))       return "Sitting";
//
//	return "Unknown Job Type";
//}
//
//void Villager::printJobQueue() const {
//	std::cout << "\n==================================================" << std::endl;
//	std::cout << "   JOB QUEUE MEMORY FOR: Villager (" << this << ")" << std::endl;
//	std::cout << "==================================================" << std::endl;
//
//	if (currentJob) {
//		std::cout << "  [ACTIVE RUNNING] -> Name: " << std::left << std::setw(15) << getJobName(currentJob)
//			<< " | Priority: " << std::fixed << std::setprecision(1) << currentJob->priority << std::endl;
//	}
//	else {
//		std::cout << "  [ACTIVE RUNNING] -> IDLE / None" << std::endl;
//	}
//
//	std::cout << "--------------------------------------------------" << std::endl;
//	std::cout << "   Paused / Interrupted Stack (Highest to Lowest):" << std::endl;
//	std::cout << "--------------------------------------------------" << std::endl;
//
//	if (interrupted.empty()) {
//		std::cout << "   (No paused jobs in memory bank)" << std::endl;
//	}
//	else {
//		for (size_t i = 0; i < interrupted.size(); ++i) {
//			std::cout << "   [" << i << "] Paused Job: " << std::left << std::setw(15) << getJobName(interrupted[i])
//				<< " | Priority: " << std::fixed << std::setprecision(1) << interrupted[i]->priority << std::endl;
//		}
//	}
//
//	std::cout << "==================================================\n" << std::endl;
//}

//void Villager::decide() {
//
//	if (threat) {
//		if (itemInHand) {
//			activity_state = ActivityState::None;
//			Job* job = new Attack(this, nullptr, SkillType::None, threat);
//			job->priority = 9999;
//
//			interrupted.push_back(currentJob);
//			currentJob = job;
//		}
//		else {
//			currentPath.clear();
//			activity_state = ActivityState::None;
//			Job* job = new Retreat(this, nullptr, SkillType::None, threat);
//			job->priority = 9999;
//
//			interrupted.push_back(currentJob);
//			currentJob = job;
//		}
//	}
//
//	if (tirednessClock > 2.0f && !sleeping) {
//		tiredness++;
//		tirednessClock = 0.0f;
//	}
//
//	if (tiredness >= 100) {
//		if (findBedClock > 1.0f) {
//			findBedClock = 0.0f;
//			claimBed();
//		}
//		activity_state = ActivityState::Sleeping;
//		auto* sleepJob = new Sleep(this, nullptr, SkillType::None);
//		sleepJob->priority = 1000;
//
//		interrupted.push_back(currentJob);
//
//		currentJob = sleepJob;
//		tiredness = 0;
//	}
//
//	if (hungerClock > 1.f) {
//		hungerClock = 0.0f;
//		hunger--;
//	}
//
//	if (hunger <= 10 && !isHungry) {
//		isHungry = true;
//	}
//
//
//	if (isHungry && findFoodClock > 2.0f && activity_state != ActivityState::Eating) {
//		findFoodClock = 0.0f;
//
//		auto foodLocation = findClosestItemType(xPos, yPos, 100, [](const Object& item, int x, int y) {
//			return item.type == Type::Food && !item.claimed;
//			//return true;
//			});
//		if (foodLocation) {
//			std::cout << "Found food at " << foodLocation->x << ", " << foodLocation->y << std::endl;
//
//			auto food = std::dynamic_pointer_cast<Food>(foodLocation->item.lock());
//			if (!food) {
//				return;
//			}
//			food->claimed = true;
//			activity_state = ActivityState::Eating;
//			Job* eat = new FindFood(this, nullptr, SkillType::None, foodLocation->x, foodLocation->y, food);
//			eat->priority = 1000;
//
//			interrupted.push_back(currentJob);
//			currentJob = eat;
//		}
//	}
//
//	if (!interrupted.empty()) {
//		currentJob = interrupted.back();
//		interrupted.pop_back();
//	}
//
//	if (currentJob) {
//		currentJob->state = JobState::Active;
//	}
//	else {
//		idle();
//	}
//}