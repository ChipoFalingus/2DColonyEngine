#include "ObjectManager.h"
#include "Tile.h"
#include "World.h"

uint64_t ObjectManager::getKey(int x, int y) const {
    return (static_cast<uint64_t>(x) << 32) | (static_cast<uint32_t>(y));
}

void ObjectManager::addObject(int x, int y, std::string object) {
    auto i = ObjectRegistry::getInstance().get(object);
    addObject(x, y, i);
}

void ObjectManager::addObject(int x, int y, std::shared_ptr<Object> object) {
    uint64_t key = getKey(x, y);
    itemMap[key].push_back(object);
    getTileRef(x, y).hasItems = true;
}

void ObjectManager::removeItem(int x, int y, std::string object) {
    uint64_t key = getKey(x, y);

    if (itemMap.count(key)) {
        auto& list = itemMap.at(key);

        auto it = std::find_if(list.begin(), list.end(),
            [&object](const std::shared_ptr<Object>& obj) {
                return obj && obj->name == object;
            });

        if (it != list.end()) {
            removeItem(x, y, *it);
        }
    }
}

void ObjectManager::removeItem(int x, int y, std::shared_ptr<Object> targetObject) {
    if (!targetObject) return;
    std::cout << "Attempting to remove " << targetObject->name << std::endl;

    auto stockpile = mainWorld.atStockpile(x, y);
    if (stockpile) {
        stockpile->removeItem(x, y, targetObject);
    }

    uint64_t key = getKey(x, y);

    if (itemMap.count(key)) {
        auto& tileVector = itemMap[key];

        auto it = std::find_if(tileVector.begin(), tileVector.end(),
            [&targetObject](const std::shared_ptr<Object>& obj) {
                return obj && obj->name == targetObject->name;
            });

        if (it != tileVector.end()) {
            tileVector.erase(it);
            std::cout << "Successfully erased one " << targetObject->name << std::endl;
        }

        if (tileVector.empty()) {
            itemMap.erase(key);
            getTileRef(x, y).hasItems = false;
            std::cout << "Vector is empty" << std::endl;
        }
    }
    getTileRef(x, y).anim.type = animType::NONE;
}

const std::vector<std::shared_ptr<Object>>& ObjectManager::getObjectsAt(int x, int y) {
    static const std::vector<std::shared_ptr<Object>> emptyResult;
    uint64_t key = getKey(x, y);

    auto it = itemMap.find(key);
    if (it != itemMap.end()) {
        return it->second;
    }
    return emptyResult;
}

void ObjectManager::clearTile(int x, int y) {
    uint64_t key = getKey(x, y);

    auto it = itemMap.find(key);
    if (it != itemMap.end()) {
        it->second.clear();
    }
}

bool ObjectManager::hasItem(int x, int y, std::string object) {
    uint64_t key = getKey(x, y);

    if (itemMap.count(key)) {
        auto& list = itemMap.at(key);

        auto it = std::find_if(list.begin(), list.end(),
            [&object](const std::shared_ptr<Object>& obj) {
                return obj && obj->name == object;
            });

        if (it != list.end()) {
            return true;
        }
    }

    return false;
}