#pragma once

#include "Tile.h"

sf::Clock animationClock;

struct RGB {
    float r, g, b;
};


RGB HSVtoRGB(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float r, g, b;

    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return { r + m, g + m, b + m };
}

sf::Color getColor(Tile& tile) {

	// Animated Colors / Animated Displays (will be added later)
    sf::Color color;

    auto& display = VisualRegistry::getInstance().get(tile.items[0]->name);

    float time = animationClock.getElapsedTime().asSeconds();

    if (tile.anim.type == BREATHE) {

		sf::Color itemColor = display.displayColor;

        float min = 0.2f;
        float max = 1.0f;

        float intensity = min + (max - min) * ((sin(time + tile.animOffset) + 1.0f) / 2.0f);

        color.r = static_cast<sf::Uint8>(itemColor.r * intensity);
        color.g = static_cast<sf::Uint8>(itemColor.g * intensity);
        color.b = static_cast<sf::Uint8>(itemColor.b * intensity);
    }

    else if (tile.anim.type == RAINBOW) {
        float hue = fmod((time * 60.0f) + tile.animOffset * 60.0f, 360.0f);
        RGB rgb = HSVtoRGB(hue, 1.0f, 1.0f);
        color.r = static_cast<sf::Uint8>(rgb.r * 255);
        color.g = static_cast<sf::Uint8>(rgb.g * 255);
		color.b = static_cast<sf::Uint8>(rgb.b * 255);
    }

    else if (tile.anim.type == RED_X) {
        tile.anim.isX = fmod(time, 1.0f) > 0.5f;

        if (tile.anim.isX) {
            tile.items[0]->displayChar = L'X';
            color = sf::Color::Red;
        }
        else {
            tile.items[0]->displayChar = display.displayChar;

            if (tile.items[0]->type == Type::Crop) {
                color = tile.items[0]->displayColor;
            }
            else {
                color = display.displayColor;
            }
        }
    }

    else if (tile.anim.type == WHITE_BREATHE) {
        sf::Color itemColor = display.displayColor;

        float speed = 2.0f;
        float wave = (sin(time * speed + tile.animOffset) + 1.0f) * 0.5f; // 0 → 1

        color.r = static_cast<sf::Uint8>(itemColor.r + (255 - itemColor.r) * wave);
        color.g = static_cast<sf::Uint8>(itemColor.g + (255 - itemColor.g) * wave);
        color.b = static_cast<sf::Uint8>(itemColor.b + (255 - itemColor.b) * wave);
    }

    return color;
}








