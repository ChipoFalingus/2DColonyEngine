#pragma once
#include "Item.h"

class Tool : public Item {
public:

	float durability;
	float efficiency;


	Tool(std::string name, wchar_t ch, sf::Color col)
		: Item(name, ch, col) {
	}
};

extern Tool axe;
extern Tool pickaxe;
extern Tool shovel;
extern Tool hoe;