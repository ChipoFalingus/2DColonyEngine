#pragma once
#include "UI/UIElements.h"
#include "Utility/Pair.h"

// I'll need to get rid of these at some point, globals are super messy and will spiral out of control

extern int xPlayer;
extern int yPlayer;

extern bool clicked;

extern bool viewMiniMap;
extern bool viewHeightMap;
extern bool viewUI;

extern int scrWidth;
extern int scrHeight;

extern double mouseX, mouseY;

extern int xFrustum;
extern int yFrustum;

extern bool placing;

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