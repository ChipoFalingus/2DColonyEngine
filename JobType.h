#pragma once
#include <string>

enum class JobType {
	None,
	Farmer,
	Miner,
	Lumberjack,
	Carpenter,
	Blacksmith,
	Gunsmith,
	Cook,
	Builder,
	Guard,
	Nomad
};

inline std::string jobTypeToString(JobType type) {
    switch (type) {
    case JobType::None:        return "None";
    case JobType::Farmer:      return "Farmer";
    case JobType::Miner:       return "Miner";
    case JobType::Lumberjack:  return "Lumberjack";
	case JobType::Carpenter:   return "Carpenter";
	case JobType::Blacksmith:  return "Blacksmith";
	case JobType::Gunsmith:    return "Gunsmith";
	case JobType::Cook:        return "Cook";
	case JobType::Builder:     return "Builder";
	case JobType::Guard:       return "Guard";
	case JobType::Nomad:       return "Nomad";
    default:                   return "Unknown";
    }
}


inline JobType stringToJobType(const std::string& type) {
    if (type == "None")        return JobType::None;
    if (type == "Farmer")      return JobType::Farmer;
    if (type == "Miner")       return JobType::Miner;
    if (type == "Lumberjack")  return JobType::Lumberjack;
    if (type == "Carpenter")   return JobType::Carpenter;
    if (type == "Blacksmith")  return JobType::Blacksmith;
    if (type == "Gunsmith")    return JobType::Gunsmith;
    if (type == "Cook")        return JobType::Cook;
    if (type == "Builder")     return JobType::Builder;
    if (type == "Guard")       return JobType::Guard;
    if (type == "Nomad")       return JobType::Nomad;
    return JobType::None;
}
