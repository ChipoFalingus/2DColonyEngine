#pragma once

#include "Villager.h"
#include "World.h"

#include <iostream>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

void save() {


	/*
		- Store seed and all world settings
		- Store changes between base seed and player additions
		- Store villagers and their stats ./
		- Store their job queues
		- Store creature world positions
		- Store squads
		- Store stockpiles and their contents ./
		- Store world items and their positions
		- Store global job queue
		- Store world time and date
	*/

	json j;
	j["seed"] = seed;
	j["world"] = {
		// Add world settings here later
	};
	j["stockpiles"] = json::array();

	for (auto& s : mainWorld.getStockpiles()) {
		json sj;
		sj["x"] = s.getLocation().first;
		sj["y"] = s.getLocation().second;
		sj["width"] = s.getWidth();
		sj["height"] = s.getHeight();
		sj["items"] = json::array();
		for (auto& item : s.getItems()) {
			json ij;
			for (auto& i : item.second)
			ij["name"] = i->name;
			sj["items"].push_back(ij);
		}
		j["stockpiles"].push_back(sj);
	}

	j["villagers"] = json::array();
	
	for (auto& v : mainWorld.getAllVillagers()) {
		json vj;
		vj["fname"] = v->firstname;
		vj["lname"] = v->lastname;
		vj["x"] = v->xPos;
		vj["y"] = v->yPos;
		vj["job"] = jobTypeToString(v->getJob());
		vj["traits"] = json::array();
		auto skills = getAllSkillTypes();
		for (int i = 0; i < skills.size(); i++) {
			vj["skills"].push_back(v->skills[skills[i]]);
		}
		/*for (int i = 0; i < v->traits.size(); i++) {
			vj["traits"].push_back(v->traits[i]);
		}*/
		j["villagers"].push_back(vj);
	}

	std::ofstream file("Saves/save.json");
	file << j.dump(4);
	file.close();
}

void read(std::string path) {
	std::ifstream;

}
