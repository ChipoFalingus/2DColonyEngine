#pragma once

#include "Utility/Pair.h"
#include "Job.h"

std::pair<int, int> findBestLightTile(int startX, int startY, float lightNeed);
std::vector<ItemLocation> findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients);
bool isAtTile(int xPos, int yPos, int xLoc, int yLoc);
std::pair<int, int> findClosestAdjTile(int xPos, int yPos, int xTile, int yTile);
std::pair<int, int> findClosestTileInRadius(int xPos, int yPos, int xTile, int yTile, int radius);
void removeBlueprint(int x, int y);
void evaluateJobDanger(Job* job);
std::optional<std::pair<int, int>> findClosestTileItem(const std::string name, int xPos, int yPos);