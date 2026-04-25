#pragma once

#include "Tile.h"


sf::Color getColor(Tile& tile) {

	// Animated Colors / Animated Displays (will be added later)
  //  sf::Color color;
  //  

  //  if (tile.anim.type == BREATHE) {
  //      float time = colorClock.getElapsedTime().asSeconds();

		//sf::Color itemColor = tile.items[0]->baseColor;

  //      float min = 0.2f;
  //      float max = 1.0f;

  //      float intensity = min + (max - min) * ((sin(time + animOffset) + 1.0f) / 2.0f);

  //      color.r = static_cast<sf::Uint8>(itemColor.r * intensity);
  //      color.g = static_cast<sf::Uint8>(itemColor.g * intensity);
  //      color.b = static_cast<sf::Uint8>(itemColor.b * intensity);
  //  }

  //  if (tile.anim.type == RAINBOW) {
  //      float time = colorClock.getElapsedTime().asSeconds();
  //      float hue = fmod((time * 60.0f) + animOffset * 60.0f, 360.0f);
  //      RGB rgb = HSVtoRGB(hue, 1.0f, 1.0f);
  //      color.r = static_cast<sf::Uint8>(rgb.r * 255);
  //      color.g = static_cast<sf::Uint8>(rgb.g * 255);
		//color.b = static_cast<sf::Uint8>(rgb.b * 255);

		//items[0]->displayColor = color;
  //  }

  //  if (tile.anim.type == RED_X) {
  //      if (animationClock.getElapsedTime().asSeconds() > 0.5f) {
  //          animationClock.restart();
  //          animationType.isX = !animationType.isX;
  //      }

  //      if (tile.anim.isX) {
  //          items[0]->displayChar = L'X';
  //          items[0]->displayColor = sf::Color::Red;
  //      }
  //      else {
  //          items[0]->displayChar = items[0]->baseChar;
  //          items[0]->displayColor = items[0]->baseColor;
  //      }
  //  }

  //  if (tile.anim.type == WHITE_BREATHE) {
  //      float time = colorClock.getElapsedTime().asSeconds();

  //      sf::Color itemColor = tile.items[0]->baseColor;

  //      float speed = 2.0f;
  //      float wave = (sin(time * speed + animOffset) + 1.0f) * 0.5f; // 0 → 1

  //      color.r = static_cast<sf::Uint8>(itemColor.r + (255 - itemColor.r) * wave);
  //      color.g = static_cast<sf::Uint8>(itemColor.g + (255 - itemColor.g) * wave);
  //      color.b = static_cast<sf::Uint8>(itemColor.b + (255 - itemColor.b) * wave);


  //      items[0]->displayColor = color;
  //  }

  //  return color;
}








