#pragma once

#include "Object.h"

#include <unordered_map>

class ObjectManager {

private:
	std::unordered_map<uint64_t, std::vector<std::shared_ptr<Object>>> itemMap;

public:
    uint64_t getKey(int x, int y) const;

    void addObject(int x, int y, std::string object);
    void addObject(int x, int y, std::shared_ptr<Object> object);

    void removeItem(int x, int y, std::string object);
    void removeItem(int x, int y, std::shared_ptr<Object> targetObject);

    const std::vector<std::shared_ptr<Object>>& getObjectsAt(int x, int y);
    void clearTile(int x, int y);
    bool hasItem(int x, int y, std::string object);
};
