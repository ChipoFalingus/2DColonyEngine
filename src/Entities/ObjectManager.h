#pragma once

#include "Object.h"

#include <unordered_map>

class ObjectManager {

private:
	std::unordered_map<uint64_t, std::vector<entt::entity>> itemMap;

public:
    uint64_t getKey(int x, int y) const;

    void addObject(int x, int y, entt::entity entity);

    void removeItem(int x, int y, entt::entity entity);

    const std::vector<entt::entity>& getObjectsAt(int x, int y);
    void clearTile(int x, int y);

	bool has(int x, int y, const std::string& itemName);
    bool isEmpty(int x, int y);

    /*template<typename T>
    bool hasComponent(int x, int y) {
        uint64_t key = getKey(x, y);
        if (itemMap.find(key) == itemMap.end()) return false;
        for (auto entity : itemMap[key]) {
            if (mainWorld.registry.try_get<T>(entity)) {
                return true;
            }
        }
        return false;
	}*/
};
