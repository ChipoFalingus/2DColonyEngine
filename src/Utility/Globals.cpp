#include "Globals.h"



int xPlayer = 0;
int yPlayer = 0;

bool clicked = false;

bool viewMiniMap = false;
bool viewHeightMap = false;
bool viewUI = true;

int xTextSpacing = 15; //15
int yTextSpacing = 22; //22

int scrWidth;
int scrHeight;

float fontSize = 1.0f; //.55

double mouseX, mouseY;

int xFrustum;
int yFrustum;

std::vector<std::pair<int, int>> buildTiles;

bool placing = false;
bool buildMode = false;
bool plantMode = false;
bool harvestMode = false;
bool stockpileMode = false;

bool enableWater = false;

std::pair<int, int> corner = { 0,0 };

int mouseTileX;
int mouseTileY;

Mode currentMode = Mode::NONE;

std::vector<std::pair<int, int>> tiles;

std::vector<std::vector<std::pair<int, int>>> field;
int fieldX;
int fieldY;

int speed = 1;