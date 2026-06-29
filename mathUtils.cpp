#include <random>
#include <iostream>
#include <unordered_set>

#include "mathUtils.h"
#include "Tile.h"
#include <queue>
#include "Pair.h"
#include "Creature.h"
#include "World.h"
#include "Structure.h"

const int PERMUTATION_SIZE = 256;
int p[PERMUTATION_SIZE * 2];
std::mt19937 rng;

float perlin(float x, float y) {
    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    int ii = x0 & 255;
    int jj = y0 & 255;

    int aa = p[p[ii] + jj];
    int ab = p[p[ii] + jj + 1];
    int ba = p[p[ii + 1] + jj];
    int bb = p[p[ii + 1] + jj + 1];

    Vec2 gradAA = getGradient(aa);
    Vec2 gradAB = getGradient(ab);
    Vec2 gradBA = getGradient(ba);
    Vec2 gradBB = getGradient(bb);

    Vec2 dAA(x - x0, y - y0);
    Vec2 dBA(x - x1, y - y0);
    Vec2 dAB(x - x0, y - y1);
    Vec2 dBB(x - x1, y - y1);

    float dotAA = gradAA.dot(dAA);
    float dotBA = gradBA.dot(dBA);
    float dotAB = gradAB.dot(dAB);
    float dotBB = gradBB.dot(dBB);

    float u = fade(sx);
    float v = fade(sy);

    float lerpX1 = lerp(dotAA, dotBA, u);
    float lerpX2 = lerp(dotAB, dotBB, u);
    float result = lerp(lerpX1, lerpX2, v);

    return result;
}

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

Vec2 getGradient(int hash) {
    static const Vec2 gradients[8] = {
        { 1, 0 }, {-1, 0}, { 0, 1 }, { 0, -1 },
        { 0.707f, 0.707f }, {-0.707f, 0.707f },
        { 0.707f, -0.707f }, {-0.707f, -0.707f }
    };
    return gradients[hash % 8];
}

float calculateMapSize() {
    return 1200.0f;
}


void initializePermutation(uint32_t seed) {
    std::vector<int> perm(PERMUTATION_SIZE);

    for (int i = 0; i < PERMUTATION_SIZE; i++)
        perm[i] = i;

    std::mt19937 rng(seed);

    std::shuffle(perm.begin(), perm.end(), rng);

    for (int i = 0; i < PERMUTATION_SIZE * 2; i++) {
        p[i] = perm[i % PERMUTATION_SIZE];
    }
}

// Psuedo-random number generators

int getRandomInt(int low, int high) {
    std::uniform_int_distribution<int> num(low, high);
    return num(rng);
}


float getRandomFloat(float low, float high) {
    std::uniform_real_distribution<float> num(low, high);
    return num(rng);
}

std::vector<std::pair<int, int>> createVoronoiMap(int amount, int left, int right, int top, int bottom) {
	std::vector<std::pair<int, int>> voronoiDots;

    for (int i = 0; i < amount; i++) {
        /*Dot dot;
        dot.ID = getRandomInt(0, 21000000);
        dot.pos = { getRandomFloat(-100, 100), getRandomFloat(-100, 100) };
        dot.direction = { getRandomFloat(-1, 1), getRandomFloat(-1, 1) };
        dot.speed = { getRandomFloat(0.1f, 1.0f), getRandomFloat(0.1f, 1.0f) };
        dot.color = sf::Color(getRandomFloat(0.1f, 1.0f), getRandomFloat(0, 255), getRandomFloat(0, 255));*/
		int x = getRandomInt(left, right);
		int y = getRandomInt(top, bottom);
		if (!getTileRef(x, y).walkable) continue;
        voronoiDots.push_back({ x, y });
    }

	return voronoiDots;
}

float findDistanceToDot(std::pair<int, int> dot, int x, int y, float jitter) {
    float jitterAmount = jitter;

    float noiseX = perlin(x * 0.1f, y * 0.1f);
    float noiseY = perlin(x * 0.1f + 100.0f, y * 0.1f + 100.0f);

    float warpedX = x + noiseX * jitterAmount;
    float warpedY = y + noiseY * jitterAmount;

    float xDist = warpedX - dot.first;
    float yDist = warpedY - dot.second;

    return xDist * xDist + yDist * yDist;
}

std::pair<int, int> findClosestVoronoiDot(std::vector<std::pair<int, int>> dots, int x, int y) {
    float shortestDist = INFINITY;
    std::pair<int, int> closestDot;
    for (int i = 0; i < dots.size(); i++) {

        float dist = findDistanceToDot(dots[i], x, y, 8.0f);
        if (shortestDist > dist) {
            shortestDist = dist;
            closestDot = dots[i];
        }
    }
    return closestDot;
}

float hashNoise(int x, int y, int seed) {
    unsigned int h = x * 374761393u + y * 668265263u + seed * 374761393u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return (h & 0xFFFFFF) / float(0xFFFFFF);
}

int heuristic(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

std::vector<std::pair<int, int>> findPath(int startX, int startY, std::pair<int, int> goal, Creature* c) {

    if (!getTileRef(goal.first, goal.second).walkable) {
        return {};
    }

    if (getTileRef(startX, startY).region != getTileRef(goal.first, goal.second).region) {
        return {};
    }

    std::pair<int, int> start = { startX, startY };

    std::priority_queue<
        std::pair<int, std::pair<int, int>>,
        std::vector<std::pair<int, std::pair<int, int>>>,
        std::greater<>
    > openSet;

    std::unordered_map<std::pair<int, int>, int, pair_hash> gScore;
    std::unordered_map<std::pair<int, int>, int, pair_hash> fScore;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> cameFrom;

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push({ fScore[start], start });

    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        { 1, 1 }, {-1, 1}, {1, -1}, {-1, -1}
    };

    while (!openSet.empty()) {
        auto current = openSet.top().second;
        openSet.pop();

        if (current == goal) {
            std::vector<std::pair<int, int>> path;
            while (cameFrom.find(current) != cameFrom.end()) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        for (auto& dir : directions) {
            int neighborX = current.first + dir.first;
            int neighborY = current.second + dir.second;
            std::pair<int, int> neighbor = { neighborX, neighborY };

            Tile& neighborTile = getTileRef(neighborX, neighborY);
            if (!neighborTile.walkable) continue;
            
            bool isBlockedByGate = false;
            if (c) {
                for (auto& i : mainWorld.objectManager.getObjectsAt(neighborX, neighborY)) {
                    if (i->type == Type::Gate) {
                        auto g = static_cast<Gate*>(i.get());
                        if (!g->getWalkability(c)) {
                            isBlockedByGate = true;
                            break;
                        }
                    }
                }

            }

            if (isBlockedByGate) continue;


            int tentativeG = gScore[current] + 1;

            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeG;
                fScore[neighbor] = tentativeG + heuristic(neighbor, goal);
                openSet.push({ fScore[neighbor], neighbor });
            }
        }
    }

    return {};
}


std::vector<std::pair<int, int>> bresenham(int x0, int y0, int x1, int y1) {
    std::vector<std::pair<int, int>> points;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        points.push_back({x0, y0});

        if (x0 == x1 && y0 == y1) break;

        int err2 = err << 1;
        if (err2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (err2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
    return points;
}


bool raycast(int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        if (!getTileRef(x0, y0).walkable) {
            return false;
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }

    return true;
}

char getArrow(int dx, int dy) {
    if (dx == 1 && dy == 0) return '>';
    if (dx == -1 && dy == 0) return '<';
    if (dx == 0 && dy == 1) return 'v';
    if (dx == 0 && dy == -1) return '^';

    if (dx == 1 && dy == 1) return '\\';
    if (dx == -1 && dy == -1) return '\\';
    if (dx == 1 && dy == -1) return '/';
    if (dx == -1 && dy == 1) return '/';

    return '.'; // no direction
}

std::vector<std::vector<std::pair<int, int>>> buildFlowField(int targetX, int targetY, int dim, Creature* c) {

    int half = dim / 2;

    std::vector<std::vector<std::pair<int, int>>> flowField(dim, std::vector<std::pair<int, int>>(dim, { 0, 0 }));
    std::vector<std::vector<float>> dist(dim, std::vector<float>(dim, FLT_MAX));

    std::vector<std::pair<int, int>> dirs = {
        {1,0}, {-1,0}, {0,1}, {0,-1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

    std::queue<std::pair<int, int>> q;

    int cx = dim / 2;
    int cy = dim / 2;

    dist[cx][cy] = 0;
    q.push({ cx, cy });

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        int x = curr.first;
        int y = curr.second;

        for (auto& dir : dirs) {
            int nx = x + dir.first;
            int ny = y + dir.second;

            if (nx < 0 || nx >= dim || ny < 0 || ny >= dim)
                continue;

            int worldX = targetX + (x - half);
            int worldY = targetY + (y - half);

            Tile& tile = getTileRef(worldX + dir.first, worldY + dir.second);
            if (!tile.walkable)
                continue;

            bool isBlockedByGate = false;
            if (c) {
                for (auto& i : mainWorld.objectManager.getObjectsAt(worldX + dir.first, worldY + dir.second)) {
                    if (i->type == Type::Gate) {
                        auto g = static_cast<Gate*>(i.get());
                        if (!g->getWalkability(c)) {
                            isBlockedByGate = true;
                            break;
                        }
                    }
                }

            }

            if (isBlockedByGate) continue;


            if (dist[nx][ny] > dist[x][y] + 1) {
                dist[nx][ny] = dist[x][y] + 1;
                q.push({ nx, ny });
            }
        }
    }


    // Set Directions next

    for (int x = 0; x < flowField.size(); x++) {
        for (int y = 0; y < flowField[0].size(); y++) {

            if (dist[x][y] == FLT_MAX) {
                flowField[x][y] = { 0, 0 };
                continue;
            }

            float bestDist = dist[x][y];
            std::pair<int, int> bestDir = { 0, 0 };

            for (auto& dir : dirs) {
                int nx = x + dir.first;
                int ny = y + dir.second;

                if (nx < 0 || nx >= dim || ny < 0 || ny >= dim)
                    continue;

                if (dist[nx][ny] == FLT_MAX)
                    continue;

                if (dist[nx][ny] < bestDist) {
                    bestDist = dist[nx][ny];
                    bestDir = dir;
                }
            }

            flowField[x][y] = bestDir;

            int worldX = targetX + (x - half);
            int worldY = targetY + (y - half);

            //getTileRef( worldX, worldY).character = getArrow(bestDir.first, bestDir.second);
            
        }
    }

    return flowField;

}

char getThreatChar(float t) {
    if (t > 80) return '#';
    if (t > 50) return 'X';
    if (t > 30) return 'x';
    if (t > 10) return '.';
    return ' ';
}


std::vector<std::vector<float>> buildThreatMap(int targetX, int targetY, int dim) {

    int half = dim / 2;

    std::vector<std::vector<float>> threat(dim, std::vector<float>(dim, 1.0f));
    std::queue<std::pair<int, int>> q;

    int cx = targetX - half;
    int cy = targetY - half;


    std::vector<std::pair<int, int>> dirs = {
        {1,0}, {-1,0}, {0,1}, {0,-1},
    };

    for (auto& i : mainWorld.getAllCreatures()) {

        Zombie* z = dynamic_cast<Zombie*>(i.get());
        if (!z) continue;

        int fx = z->xPos - cx;
        int fy = z->yPos - cy;

        if (fx >= 0 && fy >= 0 && fx < dim && fy < dim) {
            threat[fx][fy] = 100.0f; // max danger
            q.push({ fx, fy });
        }
    }

   

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        int x = curr.first;
        int y = curr.second;

        for (auto& dir : dirs) {

            int nx = curr.first + dir.first;
            int ny = curr.second + dir.second;

            if (nx < 0 || ny < 0 || nx >= dim || ny >= dim)
                continue;

            int worldX = targetX + (x - half);
            int worldY = targetY + (y - half);

            if (!getTileRef(worldX, worldY).walkable) continue;

            float newThreat = threat[x][y] - 5.0f;

            if (newThreat > threat[nx][ny]) {
                threat[nx][ny] = newThreat;
                q.push({ nx, ny });
            }
        }
    }

    return threat;
}

float bellCurve(float current, float preferred, float deviation) {
    return exp(-pow(current - preferred, 2) / (2 * pow(deviation, 2)));
}