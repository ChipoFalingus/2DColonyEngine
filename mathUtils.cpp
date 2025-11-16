#include <random>
#include <iostream>

#include "mathUtils.h"

const int PERMUTATION_SIZE = 256;
int p[PERMUTATION_SIZE * 2];
std::mt19937 rng;

float perlin(float x, float y) {
    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    int ii = x0 & 255;
    int jj = y0 & 255;

    int aa = p[p[ii] + jj];
    int ab = p[p[ii] + jj + 1];
    int ba = p[p[ii + 1] + jj];
    int bb = p[p[ii + 1] + jj + 1];

    Vec2 gradAA = getGradient(aa);
    Vec2 gradAB = getGradient(ab);
    Vec2 gradBA = getGradient(ba);
    Vec2 gradBB = getGradient(bb);

    Vec2 dAA(x - x0, y - y0);
    Vec2 dBA(x - x1, y - y0);
    Vec2 dAB(x - x0, y - y1);
    Vec2 dBB(x - x1, y - y1);

    float dotAA = gradAA.dot(dAA);
    float dotBA = gradBA.dot(dBA);
    float dotAB = gradAB.dot(dAB);
    float dotBB = gradBB.dot(dBB);

    float u = fade(sx);
    float v = fade(sy);

    float lerpX1 = lerp(dotAA, dotBA, u);
    float lerpX2 = lerp(dotAB, dotBB, u);
    float result = lerp(lerpX1, lerpX2, v);

    return result;
}

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

Vec2 getGradient(int hash) {
    static const Vec2 gradients[8] = {
        { 1, 0 }, {-1, 0}, { 0, 1 }, { 0, -1 },
        { 0.707f, 0.707f }, {-0.707f, 0.707f },
        { 0.707f, -0.707f }, {-0.707f, -0.707f }
    };
    return gradients[hash % 8];
}




float calculateMapSize() {
    return 500.0f;
}


void initializePermutation() {
    std::vector<int> perm(PERMUTATION_SIZE);
    for (int i = 0; i < PERMUTATION_SIZE; i++) {
        perm[i] = i;
    }

    // Shuffle it
    for (int i = 0; i < PERMUTATION_SIZE; i++) {
        int j = rand() % PERMUTATION_SIZE;
        std::swap(perm[i], perm[j]);
    }

    // Duplicate for overflow
    for (int i = 0; i < PERMUTATION_SIZE * 2; i++) {
        p[i] = perm[i % PERMUTATION_SIZE];
    }
}

// Psuedo-random number generators

int getRandomInt(int low, int high) {
    std::uniform_int_distribution<int> num(low, high);
    return num(rng);
}


float getRandomFloat(float low, float high) {
    std::uniform_real_distribution<float> num(low, high);
    return num(rng);
}