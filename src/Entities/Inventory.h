#pragma once
#include <entt/entt.hpp>
#include <vector>

#include "Entities/ItemComponents.h"

// Will only work with weapons and ammo for now
struct Stack {
	int stack_amount{1};
	int max{1};
};

struct Inventory {
	std::vector<entt::entity> items;
	int num_slots{ 4 };

	float clock{ 0.0f };

	bool add_to_inventory(entt::entity new_item, entt::registry& registry) {

		for (auto& e : items) {

			auto* existing_stack = registry.try_get<Stack>(e);

			if (existing_stack) {
				auto& stack_name = registry.get<Name>(e);
				auto& item_name = registry.get<Name>(new_item);
				if (existing_stack->stack_amount < existing_stack->max && stack_name.name == item_name.name) {
					existing_stack->stack_amount++;
					registry.destroy(new_item);
					return true;
				}
			}
		}

		if (items.size() < num_slots) {
			registry.emplace<Stack>(new_item);
			items.push_back(new_item);
			return true;
		}

		return false;
	}

	void remove_from_inventory(entt::entity item, entt::registry& registry) {
		for (auto it = items.begin(); it != items.end(); ++it) {
			entt::entity slot = *it;
			auto* existing_stack = registry.try_get<Stack>(slot);

			auto& stack_name = registry.get<Name>(slot);
			auto& item_name = registry.get<Name>(item);
			if (stack_name.name == item_name.name) {
				existing_stack->stack_amount--;

				if (existing_stack->stack_amount == 0) {
					entt::entity empty_slot = *it;
					items.erase(it);
				}

				break;
			}
		}


	}

	bool is_full() {
		return num_slots <= items.size();
	}
};

void updateInventories(entt::registry& registry, float deltaTime);
entt::entity hasWeapon(entt::registry& registry, entt::entity entity);