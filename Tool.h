#pragma once
#include "Item.h"


enum class ToolMaterial {
	WOOD,
	STONE,
	IRON,
	DIAMOND,
	GOLD,
	RUBY,
	EMERALD,
	OBSIDIAN,
};

inline float materialToEfficiency(ToolMaterial m) {
	switch (m) {
		case ToolMaterial::WOOD:
			return 1.0f;
		case ToolMaterial::STONE:
			return 1.5f;
		case ToolMaterial::IRON:
			return 2.0f;
		case ToolMaterial::DIAMOND:
			return 3.0f;
		case ToolMaterial::GOLD:
			return 2.5f;
		case ToolMaterial::RUBY:
			return 3.5f;
		case ToolMaterial::EMERALD:
			return 4.0f;
		case ToolMaterial::OBSIDIAN:
			return 5.0f;
		default:
			return 1.0f;
	}
	return 1.0f;
}

class Tool : public Item {
public:



	ToolMaterial material;

	float durability;
	float efficiency;
	float luck;


	Tool(std::string name, wchar_t ch, sf::Color col, ToolMaterial material)
		: Item(name, ch, col), material(material) {
	}

	void setMaterial(ToolMaterial mat) {
		material = mat;
	}
};

class ToolRegistry {
public:
	static ToolRegistry& getInstance() {
		static ToolRegistry instance;
		return instance;
	}

	void addTool(const Tool& item) {
		tools.emplace(item.name, item);
	}

	Tool* get(const std::string name) {
		return &tools.at(name);
	}

private:
	std::unordered_map<std::string, Tool> tools;
};

void loadTools();