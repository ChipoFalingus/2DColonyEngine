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

        int amount;

        std::vector<std::string> outputs;
        for (const auto& p : i.at("produces")) {
            amount = p.at("amount").get<int>();
            for (int j = 0; j < amount; j++) {
                outputs.push_back(p.at("item").get<std::string>());
            }
        }

        std::string toolRequired = i.at("tool_required").get<std::string>();
        SkillType skillType = stringToSkillType(i.at("job_required").get<std::string>());

        for (auto& j : i.at("target")) {

            std::string target = j;

            Rule rule(target, outputs, amount, toolRequired, skillType);
            HarvestRuleRegistry::getInstance().addRule(rule);
        }
    }
}