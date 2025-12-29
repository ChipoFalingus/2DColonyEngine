#pragma once
#include <iostream>
#include <unordered_map>

#include "Pair.h"


const int chunkDim = 16;


struct Chunk {
    int chunkX = 0, chunkY = 0;
    Tile tiles[chunkDim][chunkDim];
    float avgHeight;
    tileDisplay dominantDisplay = { std::vector<wchar_t>{L'?'}, std::vector<sf::Color>{sf::Color::White} };
};

extern std::unordered_map<std::pair<int, int>, Chunk, pair_hash> Chunks;

Chunk& loadOrGenerateChunk(int x, int y);
void addChunkToMiniMap(Chunk& chunk);
void updateMiniMap();