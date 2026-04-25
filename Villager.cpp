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

void Villager::doWork() {

	// Calculate new harvest speed based on tool
	if (toolInHand) {
		harvestTime = toolInHand->efficiency / materialToEfficiency(toolInHand->material);
	}
	else {
		harvestTime = 1.0f;
	}
    
	evaluateNeeds();

	if (checkThreatsClock.getElapsedTime().asSeconds() > 1.0f) {
		checkThreatsClock.restart();
		if (itemInHand) {
			auto z = findClosestCreatureType<Zombie>(xPos, yPos, alertness);
			if (z) {
				Job* job = new Attack(this, nullptr, JobType::None, z);
				job->priority = 999;
				addToJobQueue(job);
			}
		}
		else {
			auto z = findClosestCreatureType<Zombie>(xPos, yPos, alertness);
			if (z) {
				retreating = true;
				threat = z;
			}
		}
		
	}

	// Fallback idling
	if (jobQueue.empty()) {
		addToJobQueue(new Idle(this, nullptr, JobType::None));
	}
	

	currentJob = jobQueue.top();

	if (retreating && threat) {
		currentPath.clear();
		retreat(threat);
		return;
	}

	if (currentJob) { 
		currentJob->update(); 
		if (currentJob->completed) { 
			jobQueue.pop();
			delete currentJob;
			currentJob = nullptr; 
		} 
	} 
	
	if (currentJob) { 
		if (currentPath.empty() || currentJob->x != lastTargetX || currentJob->y != lastTargetY) {
			currentPath = findPath(xPos, yPos, { currentJob->x, currentJob->y });
			lastTargetX = currentJob->x;
			lastTargetY = currentJob->y;
		}
	}
	
	if (!currentPath.empty()) { 
		if (moveClock.getElapsedTime().asSeconds() > speed) {
			auto nextStep = currentPath.front(); 
			currentPath.erase(currentPath.begin()); 
			xPos = nextStep.first; 
			yPos = nextStep.second; 
			moveClock.restart();
		} 
	}
}


void Villager::evaluateNeeds() {

	if (tirednessClock.getElapsedTime().asSeconds() > 2.0f && !sleeping) {
		tiredness++;
		tirednessClock.restart();
	}


	if (tiredness >= 100) {
		std::cout << "Adding sleep job for " << firstname + " " + lastname << std::endl;
		if (findBedClock.getElapsedTime().asSeconds() > 1.0f) {
			findBedClock.restart();
			claimBed();
		}
		auto* sleepJob = new Sleep(this, nullptr, JobType::None);
		sleepJob->priority = 1000;
		jobQueue.push(sleepJob);
		tiredness = 0;
	}

	if (eatClock.getElapsedTime().asSeconds() > 4.2f) {
		eatClock.restart();
		//hunger--;
	}
	
	if (hunger <= 10 && !isHungry) {
		isHungry = true;
		Job* eat = new FindFood(this, nullptr, JobType::None);
		eat->priority = 1000;
		jobQueue.push(eat);
	}
}

void Villager::pickUpItem(std::shared_ptr<Object> item, int x, int y, Stockpile* stockpile) {
	getTileRef(x, y).removeItem(item, x, y);

	// Inventory logic goes here later
}

void Villager::dropItem(std::shared_ptr<Object> item, int x, int y) {
	getTileRef(x, y).addObject(item);

	// Adds to stockpile if dropped on one
	/*if (auto s = mainWorld.atStockpile(x, y)) {
		s->addItem(item, x, y);
	}*/

	// Inventory logic goes here later
}

void Villager::retreat(Creature* threat) {
	int safeScore = -1;

	if (!threat || threat->dead) {
		retreating = false;
		threat = nullptr;
		return;
	}

	float dx = xPos - threat->xPos;
	float dy = yPos - threat->yPos;

	float distance = sqrt(dx * dx + dy * dy);

	int dim = 64;

	auto threatMap = buildThreatMap(xPos, yPos, dim);

	int startX = xPos - dim / 2;
	int startY = yPos - dim / 2;

	float bestScore = 99999.0f;
	std::pair<int, int> bestMove = { 0, 0 };

	std::vector<std::pair<int, int>> dirs = {
		{1,0}, {0,1}, {-1,0}, {0,-1}
	};

	std::shuffle(dirs.begin(), dirs.end(), rng);

	bool foundMove = false;

	for (auto& i : dirs) {
		int nx = xPos + i.first;
		int ny = yPos + i.second;

		int fx = nx - startX;
		int fy = ny - startY;

		if (fx < 0 || fy < 0 || fx >= dim || fy >= dim)
			continue;

		if (!getTileRef(nx, ny).walkable)
			continue;

		float score = threatMap[fx][fy];

		if (lastMove != i) {
			score += 0.5f;
		}

		score += getRandomFloat(-0.2f, 0.2f);

		if (score < bestScore) {
			bestScore = score;
			bestMove = i;
			foundMove = true;
		}

		/*int cx = xPos - startX;
		int cy = yPos - startY;

		if (cx >= 0 && cy >= 0 && cx < dim && cy < dim) {
			float stayScore = threatMap[cx][cy];

			if (stayScore <= bestScore) {
				bestMove = { 0,0 };
			}
		}*/
	}

	if (foundMove && moveClock.getElapsedTime().asSeconds() > speed) {
		xPos += bestMove.first;
		yPos += bestMove.second;

		lastMove = bestMove;
		moveClock.restart();
	}

	if (distance > 20) {
		retreating = false;
		threat = nullptr;
	}
}