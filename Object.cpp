#include "Object.h"
#include "Tool.h"
#include "FoliageCrop.h"
#include "Food.h"
#include "Furnace.h"
#include "Spawner.h"
#include "Seed.h"

std::vector<Type> getAllTypes() {
	std::vector<Type> types;
	for (int i = 0; i < static_cast<int>(Type::COUNT); i++) {
		types.push_back(static_cast<Type>(i));
	}
	return types;
}

std::string itemTypeToString(Type type) {
	switch (type) {
	case Type::Item: return "Item";
	case Type::Tool: return "Tool";
	case Type::Food: return "Food";
	case Type::Crop: return "Crop";
	case Type::Foliage_Crop: return "Foliage Crop";
	case Type::Furnace: return "Furnace";
	case Type::Gun: return "Gun";
	case Type::Bench: return "Bench";
	case Type::Structure: return "Structure";
	case Type::Furniture: return "Furniture";
	case Type::Spawner: return "Spawner";
	default: return "Unknown";
	}
}

void loadObjects() {
	std::ifstream file("Object.json");
	json j;
	file >> j;

	for (auto& i : j.at("objects")) {
		std::string name = i.at("name").get<std::string>();

		auto& ch = i.at("visual").at("character");
		wchar_t displayChar = ch.is_number_integer() ? ch.get<int>() : ch.get<std::string>()[0];
		auto c = i.at("visual").at("color");
		sf::Color color(c[0], c[1], c[2]);
		VisualRegistry::getInstance().addVisual(name, Visual{ displayChar, color });

		//std::cout << "Loaded object into Visual Registry: " << name << std::endl;

		std::string type = i.at("type").get<std::string>();
		if (type == "object") {
			ObjectRegistry::getInstance().addObject(name, [name]() -> std::unique_ptr<Object> {
				auto obj = std::make_unique<Object>();
				obj->name = name;
				obj->type = Type::Item;

				return obj;
				});
		}
		else if (type == "tool") {
			ObjectRegistry::getInstance().addObject(name, [name]() -> std::unique_ptr<Object> {
				auto tool = std::make_unique<Tool>();
				tool->name = name;
				tool->type = Type::Tool;

				tool->durability = 3.0f;
				tool->efficiency = 2.0f;
				tool->luck = 0.0f;
				tool->material = ToolMaterial::WOOD;

				return tool;
				});
		}
		else if (type == "food") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto food = std::make_unique<Food>();
				food->name = name;
				food->type = Type::Food;

				food->nutrition = i.at("nutrition").get<int>();

				return food;
				});
		}
		else if (type == "crop") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto crop = std::make_unique<Crop>();
				crop->name = name;
				crop->type = Type::Crop;

				crop->growTime = i.at("grow_time").get<float>();

				std::vector<std::pair<wchar_t, sf::Color>> stages;
				for (auto& k : i.at("stages")) {
					auto c = k.at("color");
					sf::Color color(c[0], c[1], c[2]);
					std::string s = k.at("char").get<std::string>();
					wchar_t ch = s.empty() ? L'?' : (wchar_t)s[0];
					stages.push_back({ch, color});
				}

				crop->stages = stages;
				return crop;
				});
		}
		else if (type == "foliage_crop") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto fcrop = std::make_unique<FoliageCrop>();
				fcrop->name = name;
				fcrop->type = Type::Foliage_Crop;

				fcrop->produce = i.at("produce").get<std::string>();

				return fcrop;
				});
		}
		else if (type == "furnace") {
			ObjectRegistry::getInstance().addObject(name, [name]() -> std::unique_ptr<Object> {
				auto furnace = std::make_unique<Furnace>();
				furnace->name = name;
				furnace->type = Type::Furnace;

				return furnace;
				});
		}
		else if (type == "gun") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto gun = std::make_unique<Gun>();
				gun->name = name;
				gun->type = Type::Gun;

				gun->maxAmmo = i.at("max_ammo").get<int>();
				gun->damage = i.at("damage").get<int>();
				gun->range = i.at("range").get<float>();
				gun->attackCooldown = i.at("attack_cooldown").get<float>();

				return gun;
				});
		}
		else if (type == "spawner") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto spawner = std::make_unique<Spawner>();
				spawner->name = name;
				spawner->type = Type::Spawner;

				spawner->cooldown = i.at("cooldown").get<float>();
				spawner->radius = i.at("radius").get<float>();

				return spawner;
				});
		}
		else if (type == "bench") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto obj = std::make_unique<Object>();
				obj->name = name;
				obj->type = Type::Bench;

				return obj;
				});
		}
		else if (type == "structure") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto obj = std::make_unique<Object>();
				obj->name = name;
				obj->type = Type::Structure;

				return obj;
				});
		}
		else if (type == "furniture") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto obj = std::make_unique<Object>();
				obj->name = name;
				obj->type = Type::Furniture;

				return obj;
				});
		}
		else if (type == "seed") {
			ObjectRegistry::getInstance().addObject(name, [name, i]() -> std::unique_ptr<Object> {
				auto obj = std::make_unique<Seed>();
				obj->name = name;
				obj->type = Type::Seed;
				obj->cropType = i.at("grows_into").get<std::string>();
				return obj;
				});
		}
		else {
			std::cout << "Unknown object type: " << type << std::endl;
		}
	}
}