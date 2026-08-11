#pragma once
#include <iostream>
#include <random>
#include <unordered_set>
#include <glm/glm.hpp>

#include "Vec2.h"
#include "Dot.h"

class Creature;

struct Direction {
    int dx, dy;
};

struct TilePos {
	int16_t x;
	int16_t y;
};

extern std::mt19937 rng;
extern int seed;

float perlin(float x, float y);
float fade(float t);
float lerp(float a, float b, float t);
Vec2 getGradient(int hash);
void initializePermutation(uint32_t seed);

float inverseCircleFalloff(float x, float y, int dampen);
float calculateMapSize();

int getRandomInt(int low, int high);
float getRandomFloat(float low, float high);

std::vector<std::pair<int, int>> createVoronoiMap(int num, int left, int right, int top, int bottom);
std::pair<int, int> findClosestVoronoiDot(std::vector<std::pair<int, int>> dots, int x, int y);

float hashNoise(int x, int y, int seed);

int heuristic(const std::pair<int, int>& a, const std::pair<int, int>& b);
std::vector<std::pair<int, int>> findPath(int startX, int startY, std::pair<int, int> goal);

std::vector<std::pair<int, int>> bresenham(int x0, int y0, int x1, int y1);
bool raycast(int x0, int y0, int x1, int y1);

std::vector<Direction> buildFlowField(int targetX, int targetY, int dim);
std::vector<std::vector<float>> buildThreatMap(int targetX, int targetY, int dim);

float bellCurve(float current, float preferred, float deviation);
glm::vec3 normalizeRGB(glm::vec3 v);