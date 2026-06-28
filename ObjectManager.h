#pragma once

#include "Object.h"

#include <unordered_map>

class ObjectManager {

private:
	std::unordered_map<uint64_t, std::vector<std::shared_ptr<Object>>> itemMap;

public:

    uint64_t getKey(int x, int y) const {
        return (static_cast<uint64_t>(x) << 32) | (static_cast<uint32_t>(y));
    }

	void addItem(int x, int y, std::string item) {
        uint64_t key = getKey(x, y);
        auto i = ObjectRegistry::getInstance().get(item);
        itemMap[key].push_back(i);
	}

    void removeItem(int x, int y, std::shared_ptr<Object> targetObject) {
        if (!targetObject) return;

        uint64_t key = getKey(x, y);

        if (itemMap.count(key)) {
            auto& tileVector = itemMap[key];

            tileVector.erase(
                std::remove(tileVector.begin(), tileVector.end(), targetObject),
                tileVector.end()
            );

            if (tileVector.empty()) {
                itemMap.erase(key);
            }
        }
    }

    const std::vector<std::shared_ptr<Object>>& getObjectsAt(int x, int y) {
        static const std::vector<std::shared_ptr<Object>> emptyResult;
        uint64_t key = getKey(x, y);

        auto it = itemMap.find(key);
        if (it != itemMap.end()) {
            return it->second;
        }
        return emptyResult;
    }
};
