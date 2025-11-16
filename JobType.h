#pragma once
#pragma once
#include <string>

enum class JobType {
	None,
	Farmer,
	Miner,
	Lumberjack,
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
	case JobType::Blacksmith:  return "Blacksmith";
	case JobType::Gunsmith:    return "Gunsmith";
	case JobType::Cook:        return "Cook";
	case JobType::Builder:     return "Builder";
	case JobType::Guard:       return "Guard";
	case JobType::Nomad:       return "Nomad";
    default:                   return "Unknown";
    }
}
