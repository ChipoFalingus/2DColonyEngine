#include "ObjectManager.h"
#include "Tile.h"
#include "World.h"

uint64_t ObjectManager::getKey(int x, int y) const {
    return (static_cast<uint64_t>(x) << 32) | (static_cast<uint32_t>(y));
}

void ObjectManager::addObject(int x, int y, entt::entity entity) {
    if (entity == entt::null) return;
    itemMap[getKey(x, y)].push_back(entity);
}

void ObjectManager::removeItem(int x, int y, entt::entity entity) {
    uint64_t key = getKey(x, y);
    auto it = itemMap.find(key);
    if (it != itemMap.end()) {
        auto& vec = it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), entity), vec.end());

        if (vec.empty()) {
            itemMap.erase(it);
        }
    }
}

const std::vector<entt::entity>& ObjectManager::getObjectsAt(int x, int y) {
    static const std::vector<entt::entity> emptyResult;
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

bool ObjectManager::has(int x, int y, const std::string& itemName) {
    uint64_t key = getKey(x, y);
    auto it = itemMap.find(key);
    if (it != itemMap.end()) {
        for (const auto& entity : it->second) {
            if (mainWorld.registry.valid(entity) && mainWorld.registry.all_of<Name>(entity)) {
                const auto& nameComponent = mainWorld.registry.get<Name>(entity);
                if (nameComponent.name == itemName) {
                    return true;
                }
            }
        }
    }
    return false;
}