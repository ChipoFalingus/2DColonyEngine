#include "Room.h"
#include "World/World.h"


// There's a weird bug with 1 length hallways where it makes rooms that are techically considered "outside"
void RoomManager::findRoom(int startX, int startY) {

    if (getRoomAt(startX, startY)) return;
    if (!getTileRef(startX, startY).walkable) return;

    // If it doesnt start on a floor, dont bother
    if (!mainWorld.objectManager.has(startX, startY, "Stone Floor")) {
        return;
    }

    std::queue<std::pair<int, int>> q;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    q.push({ startX, startY });
    visited.insert({ startX, startY });

    int radius = 50;
    bool foundOutside = false;

    while (!q.empty()) {
        const std::pair<int, int> dirs[4] = {
            {0, 1}, {1, 0}, {-1, 0}, {0, -1}
        };

        auto& [currX, currY] = q.front();
        q.pop();

        for (auto& dir : dirs) {
            int nx = currX + dir.first;
            int ny = currY + dir.second;

            if (visited.count({ nx, ny })) continue;

			// Update these to check for components instead of specific names
            if (mainWorld.objectManager.has(nx, ny, "Stone Wall")) continue;
            if (mainWorld.objectManager.has(nx, ny, "Stone Door")) continue;
            if (getRoomAt(nx, ny)) continue;

            if (mainWorld.objectManager.has(nx, ny, "Stone Floor")) {
                q.push({ nx, ny });
                visited.insert({ nx, ny });
            }
            else {
                foundOutside = true;
                break;
            }
        }

        if (foundOutside) break;
    }

    if (foundOutside) {
        std::cout << "Outside area" << std::endl;
        if (auto r = getRoomAt(startX, startY)) {
            r->type = RoomType::Unassigned;
            removeRoom(r->ID);
        }
        return;
    }
    else {
        Room room;
        room.ID = nextRoomID++;
        for (auto& i : visited) {
            room.roomTiles.push_back(i);
        }

        addRoom(room);
        std::cout << "Created room with " << room.roomTiles.size() << " tiles starting from " << startX << ", " << startY << std::endl;
    }
}