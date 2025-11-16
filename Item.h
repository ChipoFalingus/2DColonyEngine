#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_map>


class Item {
public:

    int ID;

    std::string name;
    wchar_t displayChar;

	// Original color
    sf::Color baseColor;

    // Modified color for animations
    sf::Color displayColor;

    virtual int getDamage() const { return 1; }
    virtual float getRange() const { return 1.0f; }
	virtual float getAttackCooldown() const { return 1.0f; }

    virtual ~Item() = default;

    virtual void grow() {}

    Item() : name(""), displayChar(L' '), displayColor(sf::Color::White) {}

    Item(std::string name, wchar_t ch, sf::Color col)
        : name(name), displayChar(ch), displayColor(col) {

		baseColor = col;
    }


    bool operator==(const Item& other) const {
        return name == other.name;
    }

    bool operator!=(const Item& other) const {
        return name != other.name;
    }
   
};

namespace std {
    template<>
    struct hash<Item> {
        std::size_t operator()(const Item& item) const noexcept {
            return hash<std::string>()(item.name);
        }
    };
}



extern Item wood;
extern Item tree;
extern Item tree2;
extern Item rock;
extern Item iron;
extern Item emerald;
extern Item gold;
extern Item ruby;
extern Item sapphire;

extern Item flower;
extern Item soil;

extern Item wheatSeed;
extern Item chest;

extern Item stockPile;
extern Item wall;

extern Item DISPLAY;
extern Item EMPTY_ITEM;
extern std::unordered_map<Item, Item> harvestItems;



