#pragma once

class Item;

struct ItemLocation {
    int x, y;
    Object* item;

    ItemLocation() = default;

    ItemLocation(int px, int py, Object* pitem)
        : x(px), y(py), item(pitem) {
    }
};
