#include "Item.h"
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

//NAME, DISPLAY CHARACTER, DISPLAY COLOR


void loadCategory(const json& category) {
	
    for (auto& i : category) {
        std::string name = i.at("name").get<std::string>();

        auto& j = i.at("character");
        wchar_t displayChar;

        if (j.is_number_integer()) {
            displayChar = i.at("character").get<int>();
            std::cout << name << ": " << displayChar << std::endl;
        }
        else {
            displayChar = i.at("character").get<std::string>()[0];
        }


        //wchar_t displayChar = static_cast<wchar_t>(get]);
        //wchar_t displayChar = static_cast<wchar_t>(charStr[0]);

        auto c = i.at("color");
        sf::Color color(
            static_cast<sf::Uint8>(c.at(0).get<int>()),
            static_cast<sf::Uint8>(c.at(1).get<int>()),
            static_cast<sf::Uint8>(c.at(2).get<int>())
        );

        Item item(name, displayChar, color);
        ItemRegistry::getInstance().addItem(item);

        std::cout << "Loaded item: " << name << std::endl;
    }
}

void loadItems() {
    std::ifstream file("Item.json");
    json  data;
    file >> data;

	loadCategory(data.at("foliage"));
	loadCategory(data.at("natural_resources"));
	loadCategory(data.at("infrastructure"));
	loadCategory(data.at("letters"));
}

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

Item blood("Blood", L';', sf::Color::Red);





//std::unordered_map<Item, std::vector<std::pair<Item, float>>> harvestItems = {
//    { tree,   {{ wood, 1.0f }, { pinecone, 0.25f }} },
//    { tree2,  {{ wood, 1.0f }} },
//    { rock,   {{ rock, 1.0f }, { iron, 0.5f }, { gold, 0.05f }, {diamond, 0.005f}} },
//    { flower, {{ flower, 1.0f }, { pinecone, 0.10f }} }
//};