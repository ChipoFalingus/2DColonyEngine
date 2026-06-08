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
	Guard
};

inline std::vector<JobType> getAllJobTypes() {
    return {
        JobType::None,
        JobType::Farmer,
        JobType::Miner,
        JobType::Lumberjack,
        JobType::Carpenter,
        JobType::Blacksmith,
        JobType::Gunsmith,
        JobType::Cook,
        JobType::Builder,
        JobType::Guard
    };
}


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
    return JobType::None;
}

enum class SkillType {
    None,
    Farming,
    Mining,
    Woodcutting,
    Carpentry,
    Masonry,
    Blacksmithing,
    Gunsmithing,
    Cooking,
    Building,
    Melee,
    Shooting
};

inline std::vector<SkillType> getAllSkillTypes() {
    return {
        SkillType::Farming,
        SkillType::Mining,
        SkillType::Woodcutting,
        SkillType::Carpentry,
        SkillType::Masonry,
        SkillType::Blacksmithing,
        SkillType::Gunsmithing,
        SkillType::Cooking,
        SkillType::Building,
        SkillType::Melee,
        SkillType::Shooting
    };
}

inline std::string skillTypeToString(SkillType type) {
    switch (type) {
    case SkillType::Farming:      return "Farming";
    case SkillType::Mining:       return "Mining";
    case SkillType::Woodcutting:  return "Woodcutting";
    case SkillType::Carpentry:    return "Carpentry";
	case SkillType::Masonry:      return "Masonry";
    case SkillType::Blacksmithing:return "Blacksmithing";
    case SkillType::Gunsmithing:  return "Gunsmithing";
    case SkillType::Cooking:      return "Cooking";
    case SkillType::Building:     return "Building";
    case SkillType::Melee:        return "Melee Combat";
    case SkillType::Shooting:     return "Shooting";
    default:                     return "Unknown";
    }
}

inline SkillType stringToSkillType(const std::string& type) {
    if (type == "Farming")      return SkillType::Farming;
    if (type == "Mining")       return SkillType::Mining;
    if (type == "Woodcutting")  return SkillType::Woodcutting;
    if (type == "Carpentry")    return SkillType::Carpentry;
    if (type == "Blacksmithing")return SkillType::Blacksmithing;
    if (type == "Gunsmithing")  return SkillType::Gunsmithing;
    if (type == "Cooking")      return SkillType::Cooking;
    if (type == "Building")     return SkillType::Building;
    if (type == "Melee Combat") return SkillType::Melee;
    if (type == "Shooting")     return SkillType::Shooting;
    return SkillType::None;
}