#include "ObjectManager.h"
#include "World/Tile.h"
#include "World/World.h"

void ObjectManager::addObject(int x, int y, entt::entity entity) {
    if (entity == entt::null) return;
	m_grid[getIndex(x, y)].add(entity);
}

void ObjectManager::removeItem(int x, int y, entt::entity entity) {
    m_grid[getIndex(x, y)].remove(entity);
}

const std::vector<entt::entity> ObjectManager::getObjectsAt(int x, int y) {
	return m_grid[getIndex(x, y)].getEntities();
}

void ObjectManager::clearTile(int x, int y) {
    m_grid[getIndex(x, y)].clear();
}

bool ObjectManager::has(int x, int y, const std::string& itemName) {
    auto it = m_grid[getIndex(x, y)].getEntities();
    if (!it.empty()) {
        for (const auto& entity : it) {
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

bool ObjectManager::isEmpty(int x, int y) {
    return m_grid[getIndex(x, y)].getEntities().empty();
}