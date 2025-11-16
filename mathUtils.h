#pragma once
#include <iostream>
#include <random>
#include "Vec2.h"
#include "Dot.h"

extern std::mt19937 rng;
extern int seed;

float perlin(float x, float y);
float fade(float t);
float lerp(float a, float b, float t);
Vec2 getGradient(int hash);
void initializePermutation();

float inverseCircleFalloff(float x, float y, int dampen);
float calculateMapSize();

int getRandomInt(int low, int high);
float getRandomFloat(float low, float high);



