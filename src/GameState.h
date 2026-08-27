#pragma once
#include <utility>

enum class Mode {
	NONE,
	BUILD,
	PLANT,
	HARVEST,
	STOCKPILE
};

struct CameraState {
	int xPlayer = 0;
	int yPlayer = 0;

	int scrWidth;
	int scrHeight;

	int xFrustum;
	int yFrustum;
};

struct InputState {
	double mouseX; 
	double mouseY;

	int mouseTileX;
	int mouseTileY;
	bool clicked = false;
};

struct ViewState {
	bool viewMiniMap = false;
	bool viewHeightMap = false;
	bool viewUI = true;
};

struct PlacingState {
	std::pair<int, int> corner{0, 0};
	Mode currentMode = Mode::NONE;
	bool placing = false;
};

struct GameState {
	CameraState cameraState;
	InputState inputState;
	ViewState viewState;
	PlacingState placingState;
};
