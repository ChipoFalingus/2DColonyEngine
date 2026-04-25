#pragma once
#include <vector>
#include <SFML/Graphics/Color.hpp>

struct tileDisplay {
    std::vector<wchar_t> chars;
    std::vector<sf::Color> colors;

	tileDisplay(std::vector<wchar_t> chars, std::vector<sf::Color> colors) : chars(chars), colors(colors) {}
};