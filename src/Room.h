#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>

enum class RoomType : uint8_t {
    Unassigned,
    Bedroom,
    DiningRoom,
    Workshop,
    Storage,
    Hospital
};

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

    Room* getRoom(uint32_t id) {
        if (rooms.find(id) == rooms.end()) return nullptr;
        return &rooms[id];
    }

    Room* getRoomAt(int x, int y) {
        for (auto& [id, room] : rooms) {
            for (auto& tile : room.roomTiles) {
                if (tile.first == x && tile.second == y) {
                    return &room;
                }
            }
        }
        return nullptr;
	}

    void addRoom(const Room& room) {
        rooms[room.ID] = room;
    }

    void removeRoom(uint32_t id) {
        rooms.erase(id);
	}

    void findRoom(int startX, int startY);
    void toggleRoomView();

    void printRooms() {
        for (auto& [id, room] : rooms) {
            std::cout << "Room ID: " << id << ", Type: " << static_cast<int>(room.type) << ", Tiles: " << room.roomTiles.size() << std::endl;
        }
	}
};