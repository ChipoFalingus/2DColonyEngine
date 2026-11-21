#pragma once
#include <string>
#include <entt/entt.hpp>

enum AmmoType {
	Pistol,
	Assault_Rifle,
	Shotgun,
	UNKNOWN,
};

inline AmmoType str_to_ammo_type(std::string type) {
	if (type == "Pistol") {
		return AmmoType::Pistol;
	}
	else if (type == "Assault_Rifle") {
		return AmmoType::Assault_Rifle;
	}
	else if (type == "Shotgun") {
		return AmmoType::Shotgun;
	}
	return AmmoType::UNKNOWN;
}

struct Magazine {
	AmmoType contains;
	int max_capacity;
	int current_capacity;
};

struct Gun {
	float range;
	int damage;
	float fire_rate;

	AmmoType accepts;

	float reload_time{2.0f};

	float fire_clock{0.0f};
	float reload_clock{0.0f};
	bool reloading{false};

	entt::entity loaded_mag = entt::null;

	[[nodiscard]] bool isEmpty(entt::registry& registry) const { 
		if (loaded_mag == entt::null) return true;
		return registry.get<Magazine>(loaded_mag).current_capacity <= 0;
	}

	[[nodiscard]] bool canFire(entt::registry& registry) const {
		if (loaded_mag == entt::null) return false;
		return registry.get<Magazine>(loaded_mag).current_capacity > 0 && !reloading;
	}

	void shoot(entt::registry& registry) {
		if (loaded_mag == entt::null) return;
		registry.get<Magazine>(loaded_mag).current_capacity--;
	}
};