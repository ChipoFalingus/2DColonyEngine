#include "Tool.h"
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

// Independent from regular item list
void loadTools() {
    std::ifstream file("Tool.json");
    json  data;
    file >> data;

    for (auto& i : data.at("tools")) {
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

        auto c = i.at("color");
        sf::Color color(
            static_cast<sf::Uint8>(c.at(0).get<int>()),
            static_cast<sf::Uint8>(c.at(1).get<int>()),
            static_cast<sf::Uint8>(c.at(2).get<int>())
        );

        Tool tool(name, displayChar, color, ToolMaterial::WOOD);
		tool.durability = i.at("base_durability").get<float>();
		tool.efficiency = i.at("base_efficiency").get<float>();
		tool.luck = i.at("base_luck").get<float>();

        ToolRegistry::getInstance().addTool(tool);

        std::cout << "Loaded item: " << name << std::endl;
    }
}