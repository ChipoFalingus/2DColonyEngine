#pragma once
#include <string>
#include <glm/glm.hpp>

#include "Object.h"

enum class ToolMaterial {
	NONE,
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
			return 0.5f;
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

inline std::string materialToString(ToolMaterial m) {
	switch (m) {
		case ToolMaterial::WOOD:
			return "Wood";
		case ToolMaterial::STONE:
			return "Stone";
		case ToolMaterial::IRON:
			return "Iron";
		case ToolMaterial::DIAMOND:
			return "Diamond";
		case ToolMaterial::GOLD:
			return "Gold";
		case ToolMaterial::RUBY:
			return "Ruby";
		case ToolMaterial::EMERALD:
			return "Emerald";
		case ToolMaterial::OBSIDIAN:
			return "Obsidian";
		default:
			return "Unknown";
	}
	return "Unknown";
}

inline ToolMaterial stringToMaterial(const std::string& s) {
	if (s == "Wood") return ToolMaterial::WOOD;
	if (s == "Stone") return ToolMaterial::STONE;
	if (s == "Iron") return ToolMaterial::IRON;
	if (s == "Diamond") return ToolMaterial::DIAMOND;
	if (s == "Gold") return ToolMaterial::GOLD;
	if (s == "Ruby") return ToolMaterial::RUBY;
	if (s == "Emerald") return ToolMaterial::EMERALD;
	if (s == "Obsidian") return ToolMaterial::OBSIDIAN;
	return ToolMaterial::WOOD; // default
}

inline glm::vec3 materialToColor(ToolMaterial m) {
	switch (m) {
		case ToolMaterial::WOOD:
			return glm::vec3(153.0f / 255.0f, 102.0f / 255.0f, 51.0f / 255.0f);
		case ToolMaterial::STONE:
			return glm::vec3(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f);
		case ToolMaterial::IRON:
			return glm::vec3(179.0f / 255.0f, 179.0f / 255.0f, 179.0f / 255.0f);	
		case ToolMaterial::DIAMOND:
			return glm::vec3(51.0f / 255.0f, 204.0f / 255.0f, 255.0f / 255.0f);
		case ToolMaterial::GOLD:
			return glm::vec3(255.0f / 255.0f, 215.0f / 255.0f, 0.0f / 255.0f);
		case ToolMaterial::RUBY:
			return glm::vec3(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f);
		case ToolMaterial::EMERALD:
			return glm::vec3(0.0f / 255.0f, 255.0f / 255.0f, 128.0f / 255.0f);
		case ToolMaterial::OBSIDIAN:
			return glm::vec3(26.0f / 255.0f, 26.0f / 255.0f, 26.0f / 255.0f);
		default:
			return glm::vec3(1.0f, 1.0f, 1.0f); // white
	}
	return glm::vec3(1.0f, 1.0f, 1.0f); // white
}

class Tool : public Object {
public:
	ToolMaterial material = ToolMaterial::WOOD;

	float durability;
	float efficiency;
	float luck;


	Tool() : Object(), durability(100.0f), efficiency(1.0f), luck(0.0f) {}

	void setMaterial(ToolMaterial mat) {
		material = mat;
	}
};