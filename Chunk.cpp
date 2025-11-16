#include <iostream>
#include <unordered_map>

#include "Pair.h"
#include "Tile.h"
#include "Chunk.h"


std::unordered_map<std::pair<int, int>, Chunk, pair_hash> Chunks;

Chunk& loadOrGenerateChunk(int x, int y) {
    std::pair<int, int> pair = { x, y };
    std::map<tileType, int> typeTracker;

    float avgAlt = 0.0f;

    
    auto it = Chunks.find(pair);
    if (it == Chunks.end()) {
        Chunk newChunk;
        newChunk.chunkX = x;
        newChunk.chunkY = y;


        for (int i = 0; i < chunkDim; i++) {
            for (int j = 0; j < chunkDim; j++) {
                int worldX = x * chunkDim + i;
                int worldY = y * chunkDim + j;


                Tile tile = assignTileTypes(worldX, worldY);

                auto it = typeTracker.find(tile.type);

                if (it != typeTracker.end()) {
                    it->second += 1;
                }
                else {
					typeTracker[tile.type] = 1;
                }
                avgAlt += tile.altitude;
                newChunk.tiles[i][j] = std::move(tile);
            }
        }

        tileType type;
        int maxCount = 0;
        for (auto& i : typeTracker) {
            if (i.second > maxCount) {
                maxCount = i.second;
                type = i.first;
            }
        }
        newChunk.avgHeight = avgAlt / 256;
        newChunk.dominantDisplay = getTileDisplay(type);
        

        auto insertResult = Chunks.emplace(std::make_pair(pair, std::move(newChunk)));

        std::cout << "Chunk Rendered at " << newChunk.chunkX * chunkDim << " " << newChunk.chunkY * chunkDim << std::endl;
        return insertResult.first->second;
    }
    else {
        return it->second;
    }
}