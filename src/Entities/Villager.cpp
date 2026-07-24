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
	registry.emplace<CanAttack>(entity);

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
	updateAttack();
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

void updateAttack() {
	auto& registry = mainWorld.registry;
	auto view = registry.view<CanAttack, JobComponent, Position>();

	int alertness = 25;

	for (auto [e, attack, job, pos] : view.each()) {

		auto closestThreat = findClosestItemType(pos.x, pos.y, alertness, [&](entt::entity entity, entt::registry& reg, int x, int y) {
			return reg.try_get<Hostile>(entity) && (entity != e);
			});

		if (closestThreat.has_value()) {
			Job* attackJob = new Attack(e, entt::null, SkillType::None, closestThreat.value().item);
			attackJob->priority = 1000;
			job.proposeJob(attackJob);
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