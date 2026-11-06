#pragma once

#include "Object.h"

#include <unordered_map>

class ObjectManager {

public:

    static constexpr size_t InitialCapacity = 2;

    struct TileCell {
		entt::entity m_contents[InitialCapacity];
        std::vector<entt::entity>* m_overflow{nullptr};
        size_t m_size{ 0 };

		TileCell() = default;
        ~TileCell() { delete m_overflow; }
        TileCell(TileCell&&) noexcept = default;
        TileCell& operator=(TileCell&&) noexcept = default;

        void add(entt::entity entity) {
            if (m_size < InitialCapacity) {
                m_contents[m_size] = entity;
				m_size++;
            }
            else {
                if (!m_overflow) {
                    m_overflow = new std::vector<entt::entity>();
                }

                m_overflow->push_back(entity);
                m_size++;
            }
        }

        void remove(entt::entity entity) {
            size_t limit = std::min(m_size, InitialCapacity);
            for (int i = 0; i < limit; i++) {
                if (m_contents[i] == entity) {
                    if (limit >= InitialCapacity && m_overflow && !m_overflow->empty()) {
                        m_contents[i] = m_overflow->back();
                        m_overflow->pop_back();
                    }
                    else {
                        m_contents[i] = m_contents[limit - 1];
                    }

                    m_size--;
                    check_overflow();
                    return;
                }
            }
            if (m_overflow) {
                auto it = std::find(m_overflow->begin(), m_overflow->end(), entity);
                if (it != m_overflow->end()) {
                    m_overflow->erase(it);
                    m_size--;
                    check_overflow();
                }
            }
        }

        [[nodiscard]] bool contains(entt::entity entity) {
            size_t limit = std::min(m_size, InitialCapacity);
            for (int i = 0; i < limit; i++) {
                if (m_contents[i] == entity) return true;
            }
            if (m_overflow) {
                return std::find(m_overflow->begin(), m_overflow->end(), entity) != m_overflow->end();
            }

            return false;
        }

        [[nodiscard]] std::vector<entt::entity> getEntities() {
            std::vector<entt::entity> result;

            size_t limit = (m_size < InitialCapacity) ? m_size : InitialCapacity;
            for (int i = 0; i < limit; i++) {
                result.push_back(m_contents[i]);
            }
            if (m_overflow) {
                result.insert(result.end(), m_overflow->begin(), m_overflow->end());
            }

            return result;
        }

        [[nodiscard]] entt::entity getTopEntity() {
			if (m_size == 0) return entt::null;

            if (m_overflow && !m_overflow->empty()) {
                return m_overflow->back();
            }

            return m_contents[m_size - 1];
        }

        void clear() {
            m_size = 0;
            if (m_overflow) {
                m_overflow->clear();
            }
        }

    private:
        void check_overflow() {
            if (m_size <= InitialCapacity && m_overflow) {
                delete m_overflow;
                m_overflow = nullptr;
            }
        }
    };

    ObjectManager(int width, int height)
        : m_width(width), m_height(height), m_grid(static_cast<size_t>(width)* static_cast<size_t>(height)) {

        m_grid.resize(m_width * m_height + 1);
    }

    void addObject(int x, int y, entt::entity entity);

    void removeItem(int x, int y, entt::entity entity);

    const std::vector<entt::entity> getObjectsAt(int x, int y);
    void clearTile(int x, int y);

	bool has(int x, int y, const std::string& itemName);
    bool isEmpty(int x, int y);

    [[nodiscard]] entt::entity getTopEntity(int x, int y) {
        return m_grid[getIndex(x, y)].getTopEntity();
	}

    template<typename T>
    bool hasComponent(int x, int y, entt::registry& registry) {
		auto it = m_grid[getIndex(x, y)].getEntities();
        if (it.empty()) return false; 
        for (auto entity : it) {
            if (registry.try_get<T>(entity)) {
                return true;
            }
        }
        return false;
	}

private:
    [[nodiscard]] size_t getIndex(int x, int y) const noexcept {
        x += 500;
        y += 500;

        if (x < 0 || x > m_width) {
            //std::cout << x << " is out of bounds" << std::endl;
            return 0;
        }
        if (y < 0 || y > m_height) {
            //std::cout << y << " is out of bounds" << std::endl;
            return 0;
        }

        return static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x);
    }

    int m_minCoord{ -500 };
    int m_maxCoord{ 500 };
    int m_width{ 1000 };
    int m_height{ 1000 };

    std::vector<TileCell> m_grid;
};
