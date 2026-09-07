#include "Room.h"
#include "World/World.h"

void RoomManager::findRoom(int startX, int startY) {

    std::queue<std::pair<int, int>> q;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    q.push({ startX, startY });
    visited.insert({ startX, startY });

    int radius = 50;
    bool foundOutside = !getTileRef(startX, startY).walkable;

    std::pair<int, int> topLeft = { startX, startY };
    std::pair<int, int> bottomRight = { startX, startY };

    // If it doesnt start on a floor, dont bother
    if (!mainWorld.objectManager.has(startX, startY, "Stone Floor")) {
        return;
    }

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
            if (mainWorld.objectManager.has(nx, ny, "Stone Wall")) continue;
            if (getRoomAt(nx, ny)) continue;

            if (mainWorld.objectManager.has(nx, ny, "Stone Floor")) {
                q.push({ nx, ny });
                visited.insert({ nx, ny });

                if (topLeft.first > nx || topLeft.second < ny) {
                    topLeft = { nx, ny };
                }

                if (bottomRight.first < nx || bottomRight.second > ny) {
                    bottomRight = { nx, ny };
                }
            }
            else {
                foundOutside = true;
                break;
            }
        }
    }

    if (foundOutside) {
        std::cout << "Outside area" << std::endl;
        if (auto r = getRoomAt(startX, startY)) {
            r->type = RoomType::Unassigned;
            removeRoom(getRoomAt(startX, startY)->ID);
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
        std::cout << "Created room with " << room.roomTiles.size() << " tiles" << std::endl;
    }
}

void RoomManager::toggleRoomView() {
    for (auto& [id, room] : rooms) {
        for (auto& tile : room.roomTiles) {
            Tile& t = getTileRef(tile.first, tile.second);
            t.addObject(tile.first, tile.second, "RoomViewer");
        }
    }
}