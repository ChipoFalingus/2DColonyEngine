#include "Animal.h"

void Animal::doWork() {

    /*if (!currentJob && jobQueue.empty()) {
        addToJobQueue(new Idle(this, nullptr, JobType::None));
        return;
    }


    float elapsed = moveClock.getElapsedTime().asSeconds();
    currentJob = jobQueue.top();




    if (!currentPath.empty()) {
        if (elapsed > 1.0f / moveSpeed) {
            auto nextStep = currentPath.front();
            currentPath.erase(currentPath.begin());
            xPos = nextStep.first;
            yPos = nextStep.second;
            moveClock.restart();
        }
    }

    else if (currentJob) {
        currentJob->update();
        currentPath = findPath({ currentJob->x, currentJob->y });
        if (currentJob->completed) {
            jobQueue.pop();
            currentJob = nullptr;
        }
    }*/
};


