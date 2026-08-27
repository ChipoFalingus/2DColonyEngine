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
};
