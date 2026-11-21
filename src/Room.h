#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>

enum class RoomType : uint8_t {
    Unassigned,
    Bedroom,
    Tavern,
    Workshop,
    Storage,
    Hospital
};

inline std::string roomTypeToString(RoomType type) {
    switch (type) {
        case RoomType::Unassigned: return "Unassigned";
        case RoomType::Bedroom: return "Bedroom";
        case RoomType::Tavern: return "Tavern";
        case RoomType::Workshop: return "Workshop";
        case RoomType::Storage: return "Storage";
        case RoomType::Hospital: return "Hospital";
        default: return "Unknown";
    }
}

struct Room {
    uint32_t ID;

    std::vector<std::pair<int, int>> roomTiles;
    RoomType type = RoomType::Unassigned;
};

class RoomManager {
private:
    std::unordered_map<uint32_t, Room> rooms;
	uint32_t nextRoomID = 1;

public:

    [[nodiscard]] Room* getRoom(uint32_t id) {
        if (rooms.find(id) == rooms.end()) return nullptr;
        return &rooms[id];
    }

    [[nodiscard]] Room* getRoomAt(int x, int y) {
        for (auto& [id, room] : rooms) {
            for (auto& tile : room.roomTiles) {
                if (tile.first == x && tile.second == y) {
                    return &room;
                }
            }
        }
        return nullptr;
	}

    void removeRoomAt(int x, int y) {
        Room* room = getRoomAt(x, y);
        if (room == nullptr) return;
        removeRoom(room->ID);
    }

    void addRoom(const Room& room) {
        rooms[room.ID] = room;
    }

    void removeRoom(uint32_t id) {
        rooms.erase(id);
	}

    void findRoom(int startX, int startY);

    void printRooms() {
        for (auto& [id, room] : rooms) {
            std::cout << "Room ID: " << id << ", Type: " << static_cast<int>(room.type) << ", Tiles: " << room.roomTiles.size() << std::endl;
        }
	}
};