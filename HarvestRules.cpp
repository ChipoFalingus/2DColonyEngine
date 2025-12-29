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

        std::string produces = i.at("produces").get<std::string>();
        std::string toolRequired = i.at("tool_required").get<std::string>();
        int amount = i.at("amount").get<int>();
        JobType jobType = stringToJobType(i.at("job_required").get<std::string>());

        for (auto& j : i.at("target")) {

            std::string target = j;

            Rule rule(target, produces, amount, toolRequired, jobType);
            HarvestRuleRegistry::getInstance().addRule(rule);
        }
    }
}