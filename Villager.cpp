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
	//int rand = getRandomInt(1, 50);

	//if (rand == 0) {
	//	auto obj = ObjectRegistry::getInstance().get("Chair");
	//	auto chair = findClosestItemType(xPos, yPos, 50, [](const Object& item, int x, int y) {
	//		return item.name == "Chair" && !item.claimed;
	//		});

	//	if (chair) {
	//		chair->item.lock()->claimed = true;
	//		object_in_use = chair->item.lock().get();
	//		jobQueue.push_back(new Sit(this, nullptr, SkillType::None, chair->x, chair->y));
	//	}
	//}
	//else {
	//	//if (activity_state != ActivityState::Wandering) {
	//		activity_state = ActivityState::Wandering;
	//		//jobQueue.push_back(new Wander(this, nullptr, JobType::None));
	//	//}
	//}

	
}

void Villager::decide() {
	//if (threat) {
	//	if (itemInHand) {
	//		activity_state = ActivityState::None;
	//		Job* job = new Attack(this, nullptr, SkillType::None, threat);
	//		job->priority = 9999;
	//		addToJobQueue(job);
	//	}
	//	else {
	//		currentPath.clear();
	//		activity_state = ActivityState::None;
	//		Job* job = new Retreat(this, nullptr, SkillType::None, threat);
	//		job->priority = 9999;
	//		addToJobQueue(job);
	//	}
	//}

	//if (tirednessClock > 2.0f && !sleeping) {
	//	tiredness++;
	//	tirednessClock = 0.0f;
	//}

	//if (tiredness >= 100) {
	//	if (findBedClock > 1.0f) {
	//		findBedClock = 0.0f;
	//		claimBed();
	//	}
	//	activity_state = ActivityState::Sleeping;
	//	auto* sleepJob = new Sleep(this, nullptr, SkillType::None);
	//	sleepJob->priority = 1000;
	//	jobQueue.push_back(sleepJob);
	//	tiredness = 0;
	//}

	//if (hungerClock > 1.f) {
	//	hungerClock = 0.0f;
	//	hunger--;
	//}

	//if (hunger <= 10 && !isHungry) {
	//	isHungry = true;
	//}

	//
	//if (isHungry && findFoodClock > 2.0f && activity_state != ActivityState::Eating) {
	//	findFoodClock = 0.0f;

	//	auto foodLocation = findClosestItemType(xPos, yPos, 100, [](const Object& item, int x, int y) {
	//		return item.type == Type::Food && !item.claimed;
	//		//return true;
	//		});
	//	if (foodLocation) {
	//		std::cout << "Found food at " << foodLocation->x << ", " << foodLocation->y << std::endl;

	//		auto food = std::dynamic_pointer_cast<Food>(foodLocation->item.lock());
	//		if (!food) {
	//			return;
	//		}
	//		food->claimed = true;
	//		activity_state = ActivityState::Eating;
	//		Job* eat = new FindFood(this, nullptr, SkillType::None, foodLocation->x, foodLocation->y, food);
	//		eat->priority = 1000;
	//		jobQueue.push_back(eat);
	//	}
	//}

	/*if (!currentJob) {
		JobManager::findJobForColonist(*this);
	}*/


	if (currentJob) {
		currentJob->state = JobState::Active;
	} else {
		idle();
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
			/*for (int i = 0; i < jobQueue.size(); i++) {
				if (jobQueue[i] == currentJob) {
					jobQueue[i] = jobQueue.back();
					jobQueue.pop_back();
					break;
				}
			}*/
			currentJob = nullptr;
		}
	}

	if (!currentPath.empty()) {
		if (moveClock > speed) {
			auto nextStep = currentPath.front();
			currentPath.erase(currentPath.begin());
			xPos = nextStep.first;
			yPos = nextStep.second;
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

	/*std::sort(jobQueue.begin(), jobQueue.end(),
		[](Job* a, Job* b) {
			return a->priority > b->priority;
		});*/

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