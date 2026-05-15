#pragma once
#include <functional>

#include <iostream>
#include <fstream>
#include <SFML/Graphics/Color.hpp>

#include "json.hpp"

using json = nlohmann::json;

enum struct Type {
	Item,
	Tool,
	Food,
	Crop,
	Foliage_Crop,
	Furnace,
	Gun,
	Bench,
	Structure,
	Furniture,
	Spawner
};

struct Visual {
	wchar_t displayChar;
	sf::Color displayColor;
};


class VisualRegistry {
public:
	static VisualRegistry& getInstance() {
		static VisualRegistry instance;
		return instance;
	}
	void addVisual(const std::string& name, const Visual& visual) {
		visuals[name] = visual;
	}
	const Visual& get(const std::string& name) {
		if (visuals.find(name) == visuals.end()) {
			//std::cout << "Visual " << name << " not found in registry: " << name << std::endl;
			static Visual defaultVisual{ L'?', sf::Color::Magenta };
			return defaultVisual;
		}
		return visuals.at(name);
	}


private:
	std::unordered_map<std::string, Visual> visuals;
};

class Object {
public:
	virtual ~Object() = default;

	// Default
	std::string name;

	// Changeable
	wchar_t displayChar;
	sf::Color displayColor;

	bool claimed = false;

	Type type;

	Object() : name("") {}

	Object(std::string name, wchar_t ch, sf::Color col)
		: name(name) {
		displayChar = ch;
		displayColor = col;
	}

	virtual Visual getVisual() const {
		return VisualRegistry::getInstance().get(name);
	}
};

class ObjectRegistry {
public:
	static ObjectRegistry& getInstance() {
		static ObjectRegistry instance;
		return instance;
	}

	using Factory = std::function<std::shared_ptr<Object>()>;

	void addObject(const std::string& name, Factory factory) {
		registry[name] = factory;
	}

	std::shared_ptr<Object> get(const std::string name) {
		auto it = registry.find(name);
		if (it == registry.end()) {
			std::cout << "Object not found in registry: " << name << std::endl;
			return nullptr;
		}
		return it->second();
	}


private:
	std::unordered_map<std::string, Factory> registry;
};

struct ObjectHash {
	std::size_t operator()(const Object& obj) const noexcept {
		return std::hash<std::string>{}(obj.name);
	}
};

struct ObjectEqual {
	bool operator()(const Object& lhs, const Object& rhs) const noexcept {
		return lhs.name == rhs.name;
	}
};

void loadObjects();