#pragma once

#include "Item.h"

#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

//class Food : public Item {
//
//private:
//	float nutrition;
//	float age;
//	bool rotten;
//
//public:
//
//
//	Food(std::string name, wchar_t ch, sf::Color col, float nutrition)
//		: Item(name, ch, col), nutrition(nutrition), age(0), rotten(false) {}
//
//	float getNutrition() const override {
//		return nutrition;
//	}
//
//	void grow() override {
//		age += 1.0f;
//	}
//
//
//};
//
//void loadFoodCategory() {
//	std::ifstream file("Food.json");
//	json data;
//	file >> data;
//	for (auto& i : data.at("food")) {
//		std::string name = i.at("name").get<std::string>();
//		auto& j = i.at("character");
//		wchar_t displayChar;
//		if (j.is_number_integer()) {
//			displayChar = i.at("character").get<int>();
//		}
//		else {
//			displayChar = i.at("character").get<std::string>()[0];
//		}
//		auto c = i.at("color");
//		sf::Color color(
//			static_cast<sf::Uint8>(c.at(0).get<int>()),
//			static_cast<sf::Uint8>(c.at(1).get<int>()),
//			static_cast<sf::Uint8>(c.at(2).get<int>())
//		);
//		float nutrition = i.at("calories").get<float>();
//
//		Item item(name, displayChar, color);
//		item.nutrition = nutrition;
//
//		ItemRegistry::getInstance().addItem(item);
//		std::cout << "Loaded food item: " << name << " with calories " << nutrition << std::endl;
//	}
//}


class Food : public Object {
private:
	
public:
	int16_t nutrition;

	Food() : Object(), nutrition(1) {}

	int16_t getNutrition() {
		return nutrition;
	}
};