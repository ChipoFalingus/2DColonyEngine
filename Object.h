#pragma once
#include <functional>

#include <iostream>
#include <fstream>
#include <SFML/Graphics/Color.hpp>

#include "json.hpp"
#include <entt/entt.hpp>

#include "ItemComponents.h"

using json = nlohmann::json;

struct Blueprint {
	json data;
};

class ObjectRegistry {
private:
	std::unordered_map<std::string, Blueprint> blueprintMap;

	entt::registry staticRegistry;
public:
	static ObjectRegistry& getInstance() {
		static ObjectRegistry instance;
		return instance;
	}

	void addObject(const std::string& name, Blueprint templateEntity) {
		blueprintMap[name] = templateEntity;
	}

	entt::registry& getStaticRegistry() {
		return staticRegistry;
	}

	entt::entity getStaticObject(const std::string& name) {
		auto view = staticRegistry.view<Name>();

		for (auto entity : view) {
			const auto& nameComp = view.get<Name>(entity);

			if (nameComp.name == name) {
				return entity;
			}
		}

		return entt::null;
	}

	entt::entity createInstance(const std::string& name, entt::registry& targetWorldRegistry) {
		auto it = blueprintMap.find(name);
		if (it == blueprintMap.end()) {
			std::cout << "Object prototype not found in registry: " << name << std::endl;
			return entt::null;
		}

		entt::entity newEntity = targetWorldRegistry.create();

		targetWorldRegistry.emplace<Claimable>(newEntity, false);

		const json& blueprintData = it->second.data;

		for (auto& [name, data] : blueprintData.items()) {
			if (name == "name") {
				std::string name = data.get<std::string>();

				targetWorldRegistry.emplace<Name>(newEntity, name);
			}

			if (name == "visual") {
				wchar_t character;
				auto& chNode = data.at("character");

				if (chNode.is_number_integer()) {
					character = static_cast<wchar_t>(chNode.get<int>());
				}
				else {
					character = chNode.get<std::string>()[0];
				}

				auto& colorArray = data.at("color");
				glm::vec3 color(
					colorArray.at(0).get<int>() / 255.0f,
					colorArray.at(1).get<int>() / 255.0f,
					colorArray.at(2).get<int>() / 255.0f
				);
				targetWorldRegistry.emplace<Renderable>(newEntity, character, color);
			}

			if (name == "name") {
				std::string name = data.get<std::string>();

				targetWorldRegistry.emplace<Name>(newEntity, name);
			}

			if (name == "drop") {
				std::string dropName = data.at("item").get<std::string>();
				SkillType requiredSkill = stringToSkillType(data.at("skill").get<std::string>());
				targetWorldRegistry.emplace<Harvestable>(newEntity, dropName, requiredSkill);
			}

			if (name == "crop") {
				float growthTime = data.at("grow_time").get<float>();
				std::string produce = data.at("produce").get<std::string>();

				std::vector<std::pair<wchar_t, glm::vec3>> growthStages;
				for (auto& stage : data.at("stages")) {
					auto& chNode = stage.at("char");
					wchar_t ch = chNode.is_number_integer() ? chNode.get<int>() : chNode.get<std::string>()[0];

					auto& col = stage.at("color");
					glm::vec3 color(
						col.at(0).get<int>() / 255.0f,
						col.at(1).get<int>() / 255.0f,
						col.at(2).get<int>() / 255.0f
					);

					growthStages.push_back({ ch, color });
				}

				targetWorldRegistry.emplace<Crop>(newEntity, growthStages, produce, growthTime, 0.0f, 0, static_cast<int>(growthStages.size() - 1));

			}

			if (name == "produce") {
				std::string produce = data.get<std::string>();
				targetWorldRegistry.emplace<ProduceSpawner>(newEntity, produce);
			}

			if (name == "grows_into") {
				std::string produce = data.get<std::string>();
				targetWorldRegistry.emplace<Seed>(newEntity, produce);
			}

			if (name == "recipe") {
				std::unordered_map<std::string, int> ingredients;
				for (auto& element : data.at("ingredients")) {
					std::string ingredientName = element.at("item").get<std::string>();
					int quantity = element.at("quantity").get<int>();

					ingredients[ingredientName] = quantity;
				}

				std::string bench = data.at("required_bench").get<std::string>();

				targetWorldRegistry.emplace<Craftable>(newEntity, ingredients, bench);
			}

			if (name == "nutrition") {
				int nutrition = data.get<int>();
				targetWorldRegistry.emplace<Nutritional>(newEntity, nutrition);
			}
			
			if (name == "furniture") {
				targetWorldRegistry.emplace<Furniture>(newEntity, false);
				if (data.at("sittable").get<bool>()) {
					targetWorldRegistry.emplace<Sittable>(newEntity);
				}
				if (data.at("has_surface").get<bool>()) {
					targetWorldRegistry.emplace<Table>(newEntity);
				}
			}

			if (name == "structure") {
				targetWorldRegistry.emplace<Structure>(newEntity);
			}
		}

		return newEntity;
	}

	void loadObjects();
	void loadStaticObjects();
};
