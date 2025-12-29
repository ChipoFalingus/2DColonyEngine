#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_map>






class Item {
public:

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

class ItemRegistry {
public:
    static ItemRegistry& getInstance() {
        static ItemRegistry instance;
        return instance;
    }

    void addItem(const Item& item) {
        items.emplace(item.name, item);
    }

    Item* get(const std::string name) {
        return &items.at(name);
    }

private:
    std::unordered_map<std::string, Item> items;
};

void loadItems();



//extern Item wood;
//extern Item tree;
//extern Item tree2;
//extern Item pinecone;
//extern Item rock;
//extern Item iron;
//extern Item emerald;
//extern Item gold;
//extern Item ruby;
//extern Item sapphire;
//extern Item topaz;
//extern Item diamond;
//
//extern Item a;
//extern Item b;
//extern Item c;
//extern Item d;
//extern Item e;
//extern Item f;
//extern Item g;
//extern Item h;
//extern Item i;
//extern Item j;
//extern Item k;
//extern Item l;
//extern Item m;
//extern Item n;
//extern Item o;
//extern Item p;
//extern Item q;
//extern Item r;
//extern Item s;
//extern Item t;
//extern Item u;
//extern Item v;
//extern Item w;
//extern Item xx;
//extern Item yy;
//extern Item z;
//
//extern Item flower;
//extern Item soil;
//
//extern Item wheatSeed;
//extern Item chest;
//
//extern Item stockPile;
//extern Item wall;
//extern Item stonePath;
//extern Item woodenFence;
//
//extern Item blood;
//
//extern Item DISPLAY;
//extern Item EMPTY_ITEM;
//extern std::unordered_map<Item, std::vector<std::pair<Item, float>>> harvestItems;



