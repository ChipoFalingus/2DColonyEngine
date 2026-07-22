#pragma once


//struct Inventory {
//	//std::unordered_map<std::string, int> inventory;
//
//	std::vector<std::shared_ptr<Object>> items;
//
//	/*void add(const std::string& item, const int amount) {
//		auto it = inventory.find(item);
//
//		if (it != inventory.end()) {
//			it->second += amount;
//			return;
//		}
//		else {
//			inventory[item] = 1;
//		}
//	}
//
//	void remove(const std::string& item, const int amount) {
//		auto it = inventory.find(item);
//		if (it != inventory.end()) {
//			it->second -= amount;
//			if (it->second <= 0) {
//				inventory.erase(item);
//			}
//		}
//	}
//
//	bool has(const std::string& item) {
//		return inventory.find(item) != inventory.end();
//	}*/
//
//	void add(std::shared_ptr<Object> item) {
//		items.push_back(item);
//	}
//
//	void remove(const std::string& item) {
//		auto it = std::find_if(items.begin(), items.end(), [&](const std::shared_ptr<Object>& obj) {
//			return obj->name == item;
//			});
//		if (it != items.end()) {
//			items.erase(it);
//		}
//	}
//
//	bool has(const std::string& item) {
//		return std::any_of(items.begin(), items.end(), [&](const std::shared_ptr<Object>& obj) {
//			return obj->name == item;
//			});
//	}
//
//	std::shared_ptr<Object> get(const std::string& item) {
//		auto it = std::find_if(items.begin(), items.end(), [&](const std::shared_ptr<Object>& obj) {
//			return obj->name == item;
//			});
//		if (it != items.end()) {
//			return *it;
//		}
//		return nullptr;
//	}
//
//};