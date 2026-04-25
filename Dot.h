#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>

//enum class Biome { GRASS, DESERT };

struct Dot {
    int ID;
    Vec2 pos;
    Vec2 speed;
    Vec2 direction;
    sf::Color color;

    //Biome biome;
};