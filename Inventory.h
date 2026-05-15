#pragma once


struct Inventory {
	std::unordered_map<std::string, int> inventory;

	void add(const std::string& item, const int amount) {
		auto it = inventory.find(item);

		if (it != inventory.end()) {
			it->second += amount;
			return;
		}
		else {
			inventory[item] = 1;
		}
	}

	void remove(const std::string& item, const int amount) {
		auto it = inventory.find(item);
		if (it != inventory.end()) {
			it->second -= amount;
			if (it->second <= 0) {
				inventory.erase(item);
			}
		}
	}

	bool has(const std::string& item) {
		return inventory.find(item) != inventory.end();
	}
};