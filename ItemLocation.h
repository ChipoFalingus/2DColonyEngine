#pragma once

struct ItemLocation {
    int x, y;
    std::weak_ptr<Object> item;

    ItemLocation() = default;

    ItemLocation(int px, int py, std::weak_ptr<Object> pitem)
        : x(px), y(py), item(pitem) {
    }
};
