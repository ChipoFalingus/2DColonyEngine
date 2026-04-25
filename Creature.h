#pragma once
#include <SFML/Graphics.hpp>
#include <glm/vec3.hpp>
#include <typeindex>

#include "Item.h"
#include "Tool.h"

class Creature {
	protected:
		std::vector<std::pair<int, int>> currentPath;
		std::pair<int, int> target;
		
		float weight = 1.0f;

		
		

		int hunger = 100;
		int tiredness = 0;
		bool sleeping = false;

		int lastTargetX = 0;
		int lastTargetY = 0;

	public:
		sf::Clock clock;
		sf::Clock attackClock;
		sf::Clock repathClock;

		Creature* targetCreature = nullptr;

		int health = 100;
		int lastHealth = 100;
		wchar_t displayChar;
		glm::vec3 displayColor;


		float speed;


		int xPos;
		int yPos;

		// Creatures can hold one item type and a weapon/tool
		std::shared_ptr<Object> itemInHand;
		std::shared_ptr<Tool> toolInHand;

		bool dead = false;

		Creature(int x, int y, wchar_t glyph, glm::vec3 color)
			: xPos(x), yPos(y), displayChar(glyph), displayColor(color) {
		}

		virtual void doWork() = 0;
		virtual ~Creature() = default;


	float getDistance(Creature* other);
	void takeDamage(int dmg, Creature* attacker);

	void setCreaturePath(std::vector<std::pair<int, int>> path) {
		currentPath = path;
	}

	std::vector<std::pair<int, int>>& getCreaturePath() {
		return currentPath;
	}

};