#include "Object.h"
#include "ItemComponents.h"
#include "World.h"

void ObjectRegistry::loadObjects() {
	std::ifstream file("Object.json");
	json j;
	file >> j;

	for (auto& i : j.at("objects")) {
		std::string name = i.at("name").get<std::string>();

		blueprintMap[name] = Blueprint{ i };
	}
}

void ObjectRegistry::loadStaticObjects() {
	std::ifstream file("Object.json");
	json j;
	file >> j;
	for (auto& i : j.at("objects")) {
		entt::entity newEntity = createInstance(i.at("name").get<std::string>(), staticRegistry);
	}
}