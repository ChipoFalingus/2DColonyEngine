#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_map>

#include "Object.h"

//class Item : public Object {
//public:
//
//	bool reserved = false;
//
//	// Default stats
//    virtual int getDamage() const { return 1; }
//    virtual float getRange() const { return 1.0f; }
//	virtual float getAttackCooldown() const { return 1.0f; }
//
//	float nutrition = 0.0f;
//
//	virtual float getNutrition() const { return nutrition; }
//
//    virtual ~Item() = default;
//
//    virtual void grow() {}
//
//    bool operator==(const Item& other) const {
//        return name == other.name;
//    }
//
//    bool operator!=(const Item& other) const {
//        return name != other.name;
//    }
//   
//};
//
//namespace std {
//    template<>
//    struct hash<Item> {
//        std::size_t operator()(const Item& item) const noexcept {
//            return hash<std::string>()(item.name);
//        }
//    };
//}

//class ItemRegistry {
//public:
//    static ItemRegistry& getInstance() {
//        static ItemRegistry instance;
//        return instance;
//    }
//
//    void addItem(const Item& item) {
//        items.emplace(item.name, item);
//    }
//
//    Item* get(const std::string& name) {
//        if (items.find(name) == items.end()) {
//            std::cout << "Item not found in registry: " << name << std::endl;
//            return nullptr;
//        }
//        return &items.at(name);
//    }
//
//private:
//    std::unordered_map<std::string, Item> items;
//};
//
//void loadItems();



