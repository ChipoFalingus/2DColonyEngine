#include "HarvestRules.h"
#include "json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

void loadHarvestRules() {
    std::ifstream file("HarvestRules.json");
    json  data;
    file >> data;

    for (auto& i : data.at("rules")) {

        std::vector<Drop> outputs;
        for (const auto& p : i.at("produces")) {
            Drop drop;
            drop.drop = p.at("item").get<std::string>();
            drop.amount = p.at("amount").get<int>();
            if (p.contains("odds")) {
                drop.odds = p.at("odds").get<float>();
            }

            outputs.push_back(drop);
        }

        std::string toolRequired = i.at("tool_required").get<std::string>();
        SkillType skillType = stringToSkillType(i.at("job_required").get<std::string>());

        for (auto& j : i.at("target")) {

            std::string target = j;

            Rule rule(target, outputs, toolRequired, skillType);
            HarvestRuleRegistry::getInstance().addRule(rule);
        }
    }
}