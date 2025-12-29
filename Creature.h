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
		
		float weight;
		float speed;

		sf::Clock clock;
		sf::Clock attackClock;
		
		Creature* targetCreature;
		

		int hunger;
		int tiredness = 0;
		bool sleeping = false;
		

	public:

		static std::vector<Creature*> allCreatures;
		int health = 100;
		int lastHealth = 100;
		wchar_t displayChar;
		glm::vec3 displayColor;


		int xPos;
		int yPos;

		int xPixels;
		int yPixels;

		// Creatures can hold one item type and a weapon/tool
		Item* itemInHand;
		Tool* toolInHand;

		virtual void doWork() = 0;
		virtual ~Creature() = default;

		bool dead = false;


		Creature(int x, int y, wchar_t glyph, glm::vec3 color)
			: xPos(x), yPos(y), displayChar(glyph), displayColor(color) {
		}

	

	std::vector<std::pair<int, int>> findPath(std::pair<int, int> goal);

	


	std::vector<std::pair<int, int>> getNeighbors(int x, int y);

	bool isAtTile(int x, int y);

	void attack();

	void stop();

	int heuristic(const std::pair<int, int>& a, const std::pair<int, int>& b);
	float getDistance(Creature* other);

	static std::vector<Creature*> getCreatureList() {
		return allCreatures;
	}

};