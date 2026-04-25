#include "Crop.h"

// NAME, GROWTH STAGE, GROW TIME, STAGES
//
//Crop wheat("Wheat", 0, 2.0f,
//	{
//		{L'.', sf::Color(56, 118, 29)},
//		{L'.', sf::Color(56, 118, 29)},
//		{L'w', sf::Color(56, 118, 29)},
//		{L'W', sf::Color(147, 196, 125)},
//		{L'W', sf::Color(255, 229, 153)},
//	}
//
//);
//
//Crop carrot("Carrot", 0, 0.0f,
//	{
//		{L'.', sf::Color(56, 118, 29)},
//		{L'.', sf::Color(56, 118, 29)},
//		{L'v', sf::Color(56, 118, 29)},
//		{L'V', sf::Color(170, 255, 0)},
//		{L'V', sf::Color(255, 145, 0)},
//	}
//);
//
//Crop wacky("wacky", 0, 0.0f,
//	{
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//		{L'o', sf::Color(246, 0, 0)},
//		{L'o', sf::Color(255, 140, 0)},
//		{L'o', sf::Color(255, 238, 0)},
//		{L'o', sf::Color(77, 233, 76)},
//		{L'o', sf::Color(55, 131, 255)},
//		{L'o', sf::Color(72, 21, 170)},
//
//	}
//);