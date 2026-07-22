#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>

struct Dot {
    int ID;
    Vec2 pos;
    Vec2 speed;
    Vec2 direction;
    sf::Color color;
};