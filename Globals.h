#pragma once
#include "UIElements.h"
#include "Item.h"
#include "Pair.h"

// I'll need to get rid of these at some point, globals are super messy and will spiral out of control

extern int xPlayer;
extern int yPlayer;

extern bool clicked;

extern bool viewMiniMap;
extern bool viewHeightMap;

extern int xTextSpacing;
extern int yTextSpacing;

extern int scrWidth;
extern int scrHeight;

extern float fontSize;

extern UI ui;
extern UI buildUI;

extern double mouseX, mouseY;

extern int xFrustum;
extern int yFrustum;

extern std::vector<std::pair<int, int>> buildTiles;

extern bool placing;
extern bool buildMode;
extern bool plantMode;
extern bool harvestMode;
extern bool stockpileMode;

extern std::pair<int, int> corner;

extern int mouseTileX;
extern int mouseTileY;

struct Stockpile {
	std::pair<int, int> location;
	bool claimed = false;

	Stockpile(std::pair<int, int> loc)
		: location(loc) {
	}
};

extern std::vector<Stockpile> stockpileTiles;
extern std::vector<std::pair<Item*, std::pair<int, int>>> itemsToMove;
