#pragma once
#include "Item.h"
#include "Pair.h"
#include "mathUtils.h"

class Crop : public Object {
public:

	int growStage;
	float growTime;

	// Needs global clock
	sf::Clock growClock;

	float randomOffset;
	std::string cropName;

	std::vector<std::pair<wchar_t, sf::Color>> stages;

	Crop() : Object(), growStage(0), growTime(1.0f), randomOffset(1.0f), cropName("NULL") {}

	Visual getVisual() const override {
		const auto& stage = stages[growStage];
		return Visual{ stage.first, stage.second };
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
