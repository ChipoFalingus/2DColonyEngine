#include "Pig.h"
#include "Item.h"
#include "Tile.h"
#include "mathUtils.h"
#include "Crop.h"

Pig::Pig(int x, int y)
    : Animal(x, y, 'P', glm::vec3(255, 0, 255))
{}

float restTime = 0.5f;

void Pig::doWork() {

    /*if (currentPath.empty()) {

        if (hunger >= 100) {
			auto foodTarget = findClosestTileItem(flower);
			currentPath = findPath(foodTarget);
            if (isAtItem(flower)) {

                Tile& tile = getTileRef(foodTarget.first, foodTarget.second);
                hunger = 0;
                isHungry = false;
                changeTileItem(target.first, target.second, std::make_unique<Item>(EMPTY_ITEM));

				
            }
        }
        else {
            int targetX = xPos + (rand() % 3 - 1);
            int targetY = yPos + (rand() % 3 - 1);
            currentPath = findPath({ targetX, targetY });
        }
    }
        

    if (!currentPath.empty()) {
        auto nextStep = currentPath.front();
        currentPath.erase(currentPath.begin());
        xPos = nextStep.first;
		yPos = nextStep.second;
    }

    hunger+=1;*/
}
