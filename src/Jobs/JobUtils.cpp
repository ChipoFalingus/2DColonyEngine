#include "JobUtils.h"

#include <queue>
#include <unordered_set>
#include <utility>
#include <cmath>
#include <iostream>

#include "World/Tile.h"
#include "World/World.h"

std::pair<int, int> findBestLightTile(int startX, int startY, float lightNeed) {
	std::queue<std::pair<int, int>> q;
	std::unordered_set<std::pair<int, int>, pair_hash> visited;

	q.push({ startX, startY });
	visited.insert({ startX, startY });

	int radius = 50;

	while (!q.empty()) {
		const std::pair<int, int> dirs[4] = {
			{0, 1}, {1, 0}, {-1, 0}, {0, -1}
		};

		auto& [currX, currY] = q.front();
		q.pop();

		for (auto& dir : dirs) {
			int nx = currX + dir.first;
			int ny = currY + dir.second;

			if (visited.count({ nx, ny })) continue;
			if (!getTileRef(nx, ny).walkable) continue;

			int dx = nx - startX;
			int dy = ny - startY;

			if (dx * dx + dy * dy > radius * radius) continue;

			if (mainWorld.getLightManager().getLightMapIndex(nx, ny) >= lightNeed) {
				return { nx, ny };
			}

			q.push({ nx, ny });
			visited.insert({ nx, ny });
		}
	}

	return { startX, startY };
}

std::vector<ItemLocation> findIngredientsForJob(const std::unordered_map<std::string, int>& ingredients) {

	std::vector<ItemLocation> result;
	std::vector<entt::entity> reserved;
	bool success = true;

	for (const auto& [name, quantity] : ingredients) {
		std::cout << "Finding ingredient: " << name << " x" << quantity << std::endl;
		for (int i = 0; i < quantity; i++) {
			auto itemLocation = mainWorld.findUnclaimedItemInAllStockpile(name);
			if (!itemLocation) {
				std::cout << "Ingredient " << name << " not found in any stockpile\n";
				success = false;
				break;
			}

			if (auto claimed = mainWorld.registry.try_get<Claimable>(itemLocation->item)) {
				claimed->claimed = true;
			}

			reserved.push_back(itemLocation->item);
			result.push_back(*itemLocation);
		}
		if (!success) {
			break;
		}
	}

	if (!success) {
		for (entt::entity obj : reserved) {
			auto claimed = mainWorld.registry.try_get<Claimable>(obj);
			claimed->claimed = false;
		}
		return {};
	}

	return result;
}


bool isAtTile(int xPos, int yPos, int xLoc, int yLoc) {
	int dx = xPos - xLoc;
	int dy = yPos - yLoc;

	return std::abs(dx) + std::abs(dy) == 1;
}

std::pair<int, int> findClosestAdjTile(int xPos, int yPos, int xTile, int yTile) {
	float shortest = INFINITY;
	std::pair<int, int> closestAdj;
	for (auto& i : getNeighbors(xTile, yTile)) {
		float dx = i.first - xPos;
		float dy = i.second - yPos;
		float dist = (dx * dx) + (dy * dy);

		if (dist < shortest && getTileRef(i.first, i.second).walkable) {
			closestAdj = i;
			shortest = dist;
		}
	}

	return closestAdj;
}

std::pair<int, int> findClosestTileInRadius(int xPos, int yPos, int xTile, int yTile, int radius) {
	float shortest = INFINITY;
	std::pair<int, int> closestTile;

	const int radiusSq = radius * radius;
	for (int dx = -radius; dx <= radius; dx++) {
		for (int dy = -radius; dy <= radius; dy++) {
			int nx = xTile + dx;
			int ny = yTile + dy;
			if (!getTileRef(nx, ny).walkable) continue;
			if (dx * dx + dy * dy > radiusSq) continue;
			float dist = (nx - xPos) * (nx - xPos) + (ny - yPos) * (ny - yPos);
			if (dist < shortest) {
				closestTile = { nx, ny };
				shortest = dist;
			}
		}
	}
	return closestTile;
}

void removeBlueprint(int x, int y) {
	Tile& tile = getTileRef(x, y);
	for (auto& item : mainWorld.objectManager.getObjectsAt(x, y)) {
		if (mainWorld.registry.any_of<BlueprintTag>(item)) {
			tile.removeObject(x, y, item);
			std::cout << "Removed blueprint at " << x << ", " << y << std::endl;
			break;
		}
	}
}

void evaluateJobDanger(Job* job) {
	if (!job) return;

	auto closestThreat = findClosestItemType(job->x, job->y, 24, [&](entt::entity entity, entt::registry& reg, int x, int y) {
		return reg.try_get<Hostile>(entity) && (entity != job->villager);
		});

	if (closestThreat.has_value()) {

		// Flag nearby jobs
		const int flagRadius = 10;
		const int flagRadiusSq = flagRadius * flagRadius;

		for (auto& i : JobManager::JobList) {
			int dx = std::abs(i->x - job->x);
			int dy = std::abs(i->y - job->y);

			if (dx * dx + dy * dy <= flagRadiusSq) {
				i->state = JobState::Dangerous;
			}
		}

		job->state = JobState::Dangerous;
	}
	else {
		job->state = JobState::Queued;
	}
}

std::optional<std::pair<int, int>> findClosestTileItem(const std::string name, int xPos, int yPos) {

	int maxRadius = 100;

	std::queue<std::pair<int, int>> frontier;
	std::unordered_set<std::pair<int, int>, pair_hash> visited;

	frontier.push({ xPos, yPos });
	visited.insert({ xPos, yPos });

	while (!frontier.empty()) {
		auto current = frontier.front();
		frontier.pop();

		int dx = current.first - xPos;
		int dy = current.second - yPos;

		if (dx * dx + dy * dy > maxRadius * maxRadius)
			continue;

		Tile& tile = getTileRef(current.first, current.second);
		if (mainWorld.objectManager.has(current.first, current.second, name)) {
			//tile.claimed = true;
			return current;
		}

		for (auto& neighbor : getNeighbors(current.first, current.second)) {
			if (visited.count(neighbor) == 0) {
				visited.insert(neighbor);
				frontier.push(neighbor);
			}
		}
	}

	return std::nullopt;
}