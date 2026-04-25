#include "Item.h"
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
//
//void loadCategory(const json& category) {
//	
//    for (auto& i : category) {
//        std::string name = i.at("name").get<std::string>();
//
//        auto& j = i.at("character");
//        wchar_t displayChar;
//
//        if (j.is_number_integer()) {
//            displayChar = i.at("character").get<int>();
//        }
//        else {
//            displayChar = i.at("character").get<std::string>()[0];
//        }
//
//        auto c = i.at("color");
//        sf::Color color(
//            static_cast<sf::Uint8>(c.at(0).get<int>()),
//            static_cast<sf::Uint8>(c.at(1).get<int>()),
//            static_cast<sf::Uint8>(c.at(2).get<int>())
//        );
//
//        Item item(name, displayChar, color);
//        ItemRegistry::getInstance().addItem(item);
//
//        std::cout << "Loaded item: " << name << std::endl;
//    }
//}
//
//void loadItems() {
//    std::ifstream file("Item.json");
//    json  data;
//    file >> data;
//
//	loadCategory(data.at("foliage"));
//	loadCategory(data.at("natural_resources"));
//	loadCategory(data.at("raw_materials"));
//	loadCategory(data.at("infrastructure"));
//	loadCategory(data.at("letters"));
//}