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
extern bool viewUI;

extern int xTextSpacing;
extern int yTextSpacing;

extern int scrWidth;
extern int scrHeight;

extern float fontSize;

extern double mouseX, mouseY;

extern int xFrustum;
extern int yFrustum;

extern std::vector<std::pair<int, int>> buildTiles;

extern bool placing;
extern bool buildMode;
extern bool plantMode;
extern bool harvestMode;
extern bool stockpileMode;

extern bool enableWater;

extern std::pair<int, int> corner;

extern int mouseTileX;
extern int mouseTileY;

enum class Mode {
	NONE,
	BUILD,
	PLANT,
	HARVEST,
	STOCKPILE
};

extern Mode currentMode;

extern std::vector<std::pair<int, int>> tiles;

extern std::vector<std::vector<std::pair<int, int>>> field;
extern int fieldX;
extern int fieldY;

extern int speed;