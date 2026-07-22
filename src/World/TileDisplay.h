#pragma once
#include <vector>
#include <SFML/Graphics/Color.hpp>

struct tileDisplay {
    wchar_t character;
    sf::Color color;

	tileDisplay(wchar_t character, sf::Color color) : character(character), color(color) {}
};