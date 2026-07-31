#include "Globals.h"



int xPlayer = 0;
int yPlayer = 0;

bool clicked = false;

bool viewMiniMap = false;
bool viewHeightMap = false;
bool viewUI = true;

int scrWidth;
int scrHeight;

double mouseX, mouseY;

int xFrustum;
int yFrustum;

bool placing = false;

std::pair<int, int> corner = { 0,0 };

int mouseTileX;
int mouseTileY;

Mode currentMode = Mode::NONE;