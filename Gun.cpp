#include "Gun.h"
#include "Item.h"


// MOVE TO A JSON AT SOME POINT

//  NAME, DISPLAY CHAR, COLOR, MAX AMMO, DAMAGE, RANGE, COOLDOWN

Gun PISTOL("Pistol", L'⌐', sf::Color(200, 200, 200), 7, 10, 10.0f, 0.7f);

Gun PEPPER_GUN("Pepper Gun", L'-', sf::Color(200, 200, 200), 5, 4, 50.0f, 1.0f);

Gun ASSAULT_RIFLE("Assault Rifle", L'╬', sf::Color(200, 200, 200), 30, 7, 20.0f, 0.1f);

Gun MINIGUN("Minigun", L'M', sf::Color(200, 200, 200), 200, 100, 40.0f, 0.05f);

Gun MEGA_SNIPER("MEGA SNIPER", L'A', sf::Color(200, 200, 200), 999, 999, 100.0f, 5.0f);