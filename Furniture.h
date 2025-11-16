#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Item.h"
#include "Inventory.h"


struct Furniture {
    std::unique_ptr<Item> baseItem;
    int x, y;
    Inventory inventory;
    bool claimed = false;


    Furniture(std::unique_ptr<Item> item, int posX, int posY)
        : baseItem(std::move(item)), x(posX), y(posY) {
    }
};


extern Furniture chestFurniture;
extern Furniture bedFurniture;
extern Furniture Anvil;
extern Furniture CarpentryBench;
extern Furniture GunBench;
extern Furniture Chair;
extern Furniture Table;
