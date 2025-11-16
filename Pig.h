#pragma once
#include "Animal.h"

class Pig : public Animal {
public:
    Pig(int x, int y);
    void doWork() override;
};