#pragma once

struct ItemLocation {
    int x, y;
    entt::entity item;

    ItemLocation() = default;

    ItemLocation(int px, int py, entt::entity  pitem)
        : x(px), y(py), item(pitem) {
    }
};
