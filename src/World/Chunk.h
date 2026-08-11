#pragma once
#include <unordered_map>

#include "TileDisplay.h"
#include "Tile.h"

const int chunkDim = 16;

struct Chunk {
    int chunkX = 0, chunkY = 0;
	Tile tiles[chunkDim][chunkDim];
    float avgHeight = 0.0f;
    tileDisplay dominantDisplay{ L'?', glm::vec3(1.0f, 0.0f, 0.0f)};

	bool dirty = false;
};