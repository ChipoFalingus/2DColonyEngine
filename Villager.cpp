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
	}

	return "NULL";
}

void Villager::sense() {
	if (findBedClock > 1.0f) {
		findBedClock = 0.0f;
		claimBed();
	}

	if (checkThreatsClock > 1.0f) {
		checkThreatsClock = 0.0f;
		auto z = findClosestCreatureType<Zombie>(xPos, yPos, alertness);
		if (z) {
			threat = z;
		}
	}
}

void Villager::idle() {

	// Fallback idling stuff here
	int rand = getRandomInt(1, 50);

	/*if (rand == 1) {
		auto chair = findClosestItemType(xPos, yPos, 50, [](const Object& item, int x, int y) {
			return item.name == "Wooden Chair" && !item.claimed;
			});

		if (chair) {
			chair->item.lock()->claimed = true;
			object_in_use = chair->item.lock().get();
			currentJob = new Sit(this, nullptr, SkillType::None, chair->x, chair->y);
		}
	}
	else {
		if (activity_state != ActivityState::Wandering) {
			activity_state = ActivityState::Wandering;
			currentJob = new Wander(this, nullptr, SkillType::None);
		}
	}*/

	
}

enum UtilityType {
	ATTACK,
	RETREAT,
	SLEEP,
	EAT,

};

struct Score {
	UtilityType type;
	float score;
};

//void Villager::decide() {
//
//	std::vector<Score> possibleJobs;
//
//	if (threat) {
//		possibleJobs.push_back({ ATTACK, 9990 });
//		if (!itemInHand) {
//			possibleJobs.push_back({ RETREAT, 9999 });
//		}
//	}
//
//
//	if (tirednessClock > 2.0f && !sleeping) {
//		tiredness++;
//		tirednessClock = 0.0f;
//
//		claimBed();
//	}
//
//	possibleJobs.push_back({ SLEEP, tiredness * 1.0f});
//
//
//	if (hungerClock > 1.f) {
//		hungerClock = 0.0f;
//		hunger--;
//	}
//
//	if (hunger < 80 && findFoodClock > 2.0f) {
//		findFoodClock = 0.0f;
//
//		auto foodLocation = findClosestItemType(xPos, yPos, 50, [](const Object& obj, int x, int y) {
//			return obj.type == Type::Food && !obj.claimed;
//			});
//
//		if (foodLocation) {
//			foodLocation->item.lock()->claimed = true;
//			possibleJobs.push_back({ EAT, (100 - hunger) * 10.0f });
//		}
//	}
//
//	std::sort(possibleJobs.begin(), possibleJobs.end(), [](const Score& a, const Score& b) {
//		return a.score > b.score;
//		});
//
//	if (currentJob) {
//		currentJob->state = JobState::Active;
//	} else {
//		if (!interrupted.empty()) {
//			currentJob = interrupted.back();
//			interrupted.pop_back();
//		} else {
//			auto best = possibleJobs.front();
//			switch (best.type)
//			{
//			case ATTACK:
//				currentJob = new Attack(this, nullptr, SkillType::None, threat);
//				break;
//
//			case RETREAT:
//				currentJob = new Retreat(this, nullptr, SkillType::None, threat);
//				break;
//
//			case SLEEP:
//				currentJob = new Sleep(this, nullptr, SkillType::None);
//				break;
//			}
//		}
//	}
//}

void Villager::decide() {

	if (threat) {
		if (itemInHand) {
			activity_state = ActivityState::None;
			Job* job = new Attack(this, nullptr, SkillType::None, threat);
			job->priority = 9999;

			interrupted.push_back(currentJob);
			currentJob = job;
		}
		else {
			currentPath.clear();
			activity_state = ActivityState::None;
			Job* job = new Retreat(this, nullptr, SkillType::None, threat);
			job->priority = 9999;

			interrupted.push_back(currentJob);
			currentJob = job;
		}
	}

	if (tirednessClock > 2.0f && !sleeping) {
		tiredness++;
		tirednessClock = 0.0f;
	}

	if (tiredness >= 100) {
		if (findBedClock > 1.0f) {
			findBedClock = 0.0f;
			claimBed();
		}
		activity_state = ActivityState::Sleeping;
		auto* sleepJob = new Sleep(this, nullptr, SkillType::None);
		sleepJob->priority = 1000;

		interrupted.push_back(currentJob);

		currentJob = sleepJob;
		tiredness = 0;
	}

	if (hungerClock > 1.f) {
		hungerClock = 0.0f;
		hunger--;
	}

	if (hunger <= 10 && !isHungry) {
		isHungry = true;
	}


	if (isHungry && findFoodClock > 2.0f && activity_state != ActivityState::Eating) {
		findFoodClock = 0.0f;

		auto foodLocation = findClosestItemType(xPos, yPos, 100, [](const Object& item, int x, int y) {
			return item.type == Type::Food && !item.claimed;
			//return true;
			});
		if (foodLocation) {
			std::cout << "Found food at " << foodLocation->x << ", " << foodLocation->y << std::endl;

			auto food = std::dynamic_pointer_cast<Food>(foodLocation->item.lock());
			if (!food) {
				return;
			}
			food->claimed = true;
			activity_state = ActivityState::Eating;
			Job* eat = new FindFood(this, nullptr, SkillType::None, foodLocation->x, foodLocation->y, food);
			eat->priority = 1000;

			interrupted.push_back(currentJob);
			currentJob = eat;
		}
	}

	if (currentJob) {
		currentJob->state = JobState::Active;
	}
	else {
		idle();
		if (!interrupted.empty()) {
			currentJob = interrupted.back();
			interrupted.pop_back();
		}
		else {
			idle();
		}
	}
}

void Villager::move() {

	if (currentJob) {
		currentJob->update();

		if (currentPath.empty() || currentJob->x != lastTargetX || currentJob->y != lastTargetY) {
			currentPath = findPath(xPos, yPos, { currentJob->x, currentJob->y });
			lastTargetX = currentJob->x;
			lastTargetY = currentJob->y;
		}

		if (currentJob->state == JobState::Completed) {
			currentJob = nullptr;
		}
	}

	if (!currentPath.empty()) {
		if (moveClock > speed) {
			auto nextStep = currentPath.front();
			currentPath.erase(currentPath.begin());
			if (!getTileRef(nextStep.first, nextStep.second).walkable) {
				currentPath = findPath(xPos, yPos, { currentJob->x, currentJob->y });
				return;
			}
			else {
				xPos = nextStep.first;
				yPos = nextStep.second;
			}
			moveClock = 0.0f;
		}
	}
}

void Villager::doWork() {

	clock += Clock::deltaTime;
	moveClock += Clock::deltaTime;
	idleClock += Clock::deltaTime;
	findFoodClock += Clock::deltaTime;
	checkThreatsClock += Clock::deltaTime;
	hungerClock += Clock::deltaTime;
	findBedClock += Clock::deltaTime;
	tirednessClock += Clock::deltaTime;

	if (toolInHand) {
		harvestTime = toolInHand->efficiency / materialToEfficiency(toolInHand->material);
	}
	else {
		harvestTime = 1.0f;
	}

	sense();
	decide();
	move();
}

void Villager::pickUpItem(std::shared_ptr<Object> item, int x, int y, Stockpile* stockpile) {
	getTileRef(x, y).removeItem(item, x, y);
	inventory.add(item);
}

void Villager::dropItem(std::shared_ptr<Object> item, int x, int y) {
	if (!inventory.has(item->name)) return;
	inventory.remove(item->name);
	getTileRef(x, y).addObject(item);
}