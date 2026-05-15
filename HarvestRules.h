#pragma once
#include <string>
#include <vector>
#include "JobType.h"

class Rule {
public:
	std::string target;
	std::vector<std::string> produces;
	int amount;
	std::string toolRequired;
	JobType jobType;

	Rule(std::string target, std::vector<std::string> produces, int amount, std::string toolRequired, JobType jobType)
		: target(target), produces(produces), amount(amount), toolRequired(toolRequired), jobType(jobType) {
	}
};

class HarvestRuleRegistry {
public:
	static HarvestRuleRegistry& getInstance() {
		static HarvestRuleRegistry instance;
		return instance;
	}

	void addRule(const Rule& rule) {
		rules.push_back(rule);
	}

    Rule* get(const std::string& target) {
		for (auto& r : rules) {
			if (r.target == target) {
				return &r;
			}
		}
		return nullptr;
    }


private:
	std::vector<Rule> rules;
};

void loadHarvestRules();