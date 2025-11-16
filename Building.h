#pragma once
#include "Pair.h"

enum class BuildingType { HOUSE, MILL, OUTPOST, SHOP, TAVERN, FARM, TOWER, WALL};

class Building {
public:


	BuildingType type;
	std::pair<int, int> location;
	int length;
	int width;

	std::string name;


	Building(BuildingType t, std::pair<int, int> loc, int l, int w) : type(t), location(loc), length(l), width(w) {}




};