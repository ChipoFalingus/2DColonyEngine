#include "Globals.h"

int xPlayer = 0;
int yPlayer = 0;

bool clicked = false;

bool viewMiniMap = false;
bool viewHeightMap = false;

int xTextSpacing = 15; //15
int yTextSpacing = 22; //22


// standard 1080p
int scrWidth = 1920;
int scrHeight = 1080;

float fontSize = 0.55f; //24

UI ui;
UI buildUI;

double mouseX, mouseY;

int xFrustum = scrWidth / xTextSpacing;
int yFrustum = scrHeight / yTextSpacing;

std::vector<std::pair<int, int>> buildTiles;

bool placing = false;
bool buildMode = false;
bool plantMode = false;
bool harvestMode = false;
bool stockpileMode = false;

std::pair<int, int> corner = { 0,0 };

int mouseTileX;
int mouseTileY;


std::vector<Stockpile> stockpileTiles;
std::vector<std::pair<Item*, std::pair<int, int>>> itemsToMove;