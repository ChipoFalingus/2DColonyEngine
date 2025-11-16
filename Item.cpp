#include "Item.h"
//#include "Furniture.h"

//WILL MOVE TO A JSON AT SOME POINT

//NAME, DISPLAY CHARACTER, DISPLAY COLOR

//PLANTS / FOLIAGE
Item tree("Oak Tree", L'♣', sf::Color::Green);
Item tree2("Pine Tree", L'♠', sf::Color(0, 100, 0));
Item flower("Flower", L'*', sf::Color(0, 100, 0));

Item soil("Soil", L'=', sf::Color(84, 30, 0));



//NATURAL RESOURCES
Item wood("Wood", L'O', sf::Color(139, 69, 19));
Item rock("Rock", L'R', sf::Color(128, 128, 128));

Item iron("Iron", L'O', sf::Color(217, 201, 147));
Item emerald("Emerald", L'o', sf::Color(80, 200, 120));
Item gold("Gold", L'o', sf::Color(255, 255, 0));
Item ruby("Ruby", L'o', sf::Color(255, 0, 0));
Item sapphire("Sapphire", L'o', sf::Color(0, 0, 255));

Item wheatSeed("Wheat Seed", L'.', sf::Color(56, 118, 29));

//FURNITURE
Item chest("Chest", L'C', sf::Color(100, 65, 23));


//BUILDING / INFRASTRUCTURE
Item stockPile("Stockpile", L'=', sf::Color(100, 100, 100));
Item wall("Wall", L'%', sf::Color(100, 65, 23));

//DECORATIVE
Item stonePath("Stone Path", L'o', sf::Color(169, 169, 169));
Item woodenFence("Wooden Fence", L'#', sf::Color(160, 82, 45));

//MISC.
Item DISPLAY("Display", L'#', sf::Color::White);
Item EMPTY_ITEM("IF YOU SEE THIS PLEASE TELL ME", L' ', sf::Color::Transparent);



std::unordered_map<Item, Item> harvestItems = {
	{tree, wood}
};