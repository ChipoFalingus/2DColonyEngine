#pragma once
#include "Item.h"
#include "Pair.h"
#include "mathUtils.h"

class Crop : public Item {
public:

	int growStage;
	float growTime;
	sf::Clock growClock;
	float randomOffset;
	std::string cropName;

	std::vector<std::pair<wchar_t, sf::Color>> stages;

	Crop(std::string cropName, int growStage, float growTime, std::vector<std::pair<wchar_t, sf::Color>> stages) :
		cropName(name), growStage(growStage), growTime(growTime), stages(stages) {
		
		randomOffset = getRandomFloat(0.0f, 1.0f);
		displayChar = stages[0].first;
		displayColor = stages[0].second;

		name = cropName;
	}

	void grow() {
		if (growStage < stages.size() - 1) {
			float elapsed = growClock.getElapsedTime().asSeconds();

			if (elapsed > growTime + randomOffset) {
				growStage++;
				growClock.restart();

				randomOffset = getRandomFloat(0.0f, 2.0f);

				displayChar = stages[growStage].first;
				displayColor = stages[growStage].second;
			}
		}
	}

	bool isGrown() {
		return growStage + 1 >= stages.size();
	}

};

extern Crop wheat;
extern Crop carrot;
extern Crop wacky;
