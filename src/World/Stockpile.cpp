#include "Stockpile.h"
#include "World.h"

std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> Stockpile::getItems() {
    return tileItems;
}

/*std::unordered_map<std::string, int> getFilteredItemCounts(Type type) {
    std::unordered_map<std::string, int> result;
    for (auto& [loc, f] : tileItems) {
        for (auto& item : f) {
            if (item && item->type == type) {
                result[item->name]++;
            }
        }
    }
    return result;
}*/

std::unordered_map<std::string, int> Stockpile::getItemCounts() {
    std::unordered_map<std::string, int> result;
    for (auto& [loc, f] : tileItems) {
        for (auto& item : f) {
            const auto& str = mainWorld.registry.try_get<Name>(item);
            if (str) {
                result[str->name]++;
            }
        }
    }
    return result;
}

// Checks if the given coordinates are within the bounds of the stockpile
bool Stockpile::atStockpile(int x, int y) const {
    return x >= location.first &&
        x < location.first + width &&
        y >= location.second &&
        y < location.second + height;
}

// Checks if the tile at the given coordinates is claimed
bool Stockpile::claimed(int x, int y) const {
    return claimedTiles.find({ x, y }) != claimedTiles.end();
}

// Unclaims the tile at the given coordinates, allowing it to be used again
void Stockpile::releaseTile(int x, int y) {
    claimedTiles.erase({ x, y });
}

// Adds an item to the given location. The tile is claimed if it isn't already, and the item is added to the stockpile's item list
void Stockpile::addItem(entt::entity obj, int x, int y) {
    if (obj == entt::null) {
        return;
    }
    claimedTiles.insert({ x, y });
}

void Stockpile::placeItem(entt::entity obj, int x, int y) {
    if (obj == entt::null) {
        std::cout << "Error: Tried to place null item in stockpile\n";
        return;
    }
    std::cout << "Placing item in stockpile at: " << x << "," << y << std::endl;
    tileItems[{ x, y }].push_back(obj);
}

// Removes the given item from the given location. If there are no more items at that location, it is unclaimed and removed from the stockpile's item list
void Stockpile::removeItem(int x, int y, entt::entity item) {
    auto it = tileItems.find({ x, y });
    if (it == tileItems.end())
        return;

    auto& vec = it->second;

    vec.erase(
        std::remove(vec.begin(), vec.end(), item),
        vec.end()
    );

    tileItems.erase(it);
    claimedTiles.erase({ x, y });
}

// Returns the item at a given location, if it exists
entt::entity Stockpile::retrieveItem(int x, int y) {
    auto it = tileItems.find({ x, y });
    if (it != tileItems.end()) {
        return it->second[0];
    }
    return entt::null;
}

// Finds the first open spot in the stockpile and returns its coordinates
std::optional<std::pair<int, int>> Stockpile::findOpenSpot() {
    for (int x = location.first; x < location.first + width; x++) {
        for (int y = location.second; y < location.second + height; y++) {
            if (!claimed(x, y)) {
                std::cout << "Found open spot at: " << x << "," << y << "\n";

                return std::make_pair(x, y);
            }
        }
    }
    return std::nullopt;
}

// Finds the first item with the given name and returns its coordinates
std::optional<std::pair<int, int>> Stockpile::findItemLocation(const std::string& name) {
    for (auto& [pos, item] : tileItems) {
        const auto& str = mainWorld.registry.get<Name>(item[0]);
        if (str.name == name) {
            std::cout << "Found item!\n";
            return pos;
        }
    }
    return std::nullopt;
}

// Finds the first unclaimed item with the given name and returns its coordinates
std::optional<std::pair<int, int>> Stockpile::findUnclaimedItemLocation(const std::string& name) {
    std::cout << "Looking for: " << name << "\n";

    for (auto& [pos, item] : tileItems) {
        const auto& str = mainWorld.registry.get<Name>(item[0]);
        bool claimed = mainWorld.registry.get<Claimable>(item[0]).claimed;
        if (str.name == name && !claimed) {
            std::cout << "Found item!\n";
            return pos;
        }
    }
    return std::nullopt;
}

void Stockpile::printContents() {
    std::cout << "Stockpile at (" << location.first << "," << location.second << ") contains:" << std::endl;
    for (auto& [pos, items] : tileItems) {
        for (auto& item : items) {
            const auto& str = mainWorld.registry.get<Name>(item);
            std::cout << " - " << str.name << " at (" << pos.first << "," << pos.second << ")" << std::endl;
        }
    }
}