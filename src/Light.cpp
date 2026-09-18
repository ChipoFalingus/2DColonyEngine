#include <algorithm>

#include "Utility/Light.h"
#include "Utility/mathUtils.h"
#include "World/Tile.h"
#include "World/World.h"

void LightManager::initLightMap() {
    int size = calculateMapSize() * 2 + 1;

    lightMap.resize(size * size);
    std::fill(lightMap.begin(), lightMap.end(), 0.2f);
}

const std::vector<float>& LightManager::getLightMap() {
    return lightMap;
}

float LightManager::getLightMapIndex(int x, int y) {
    const int r = calculateMapSize();
    x += r;
    y += r;
    int size = r * 2 + 1;
    if (x < 0 || x >= size || y < 0 || y >= size)
        return 0.2f;

    return std::max(mainWorld.dayCycle.getDaylightFactor(), lightMap[x + y * size]);
}


void LightManager::addLight(const glm::vec2& pos, const glm::vec3& col, float rad, float intensity, float lifetime) {
    lights.emplace_back(pos, col, rad, intensity, lifetime);
    dirty = true;
}

void LightManager::clearLights() {
    lights.clear();
}

void LightManager::update(float deltaTime) {
    for (int i = lights.size() - 1; i >= 0; i--) {
        if (lights[i].lifetime >= 0.0f) {
            lights[i].lifetime -= deltaTime;
            if (lights[i].lifetime <= 0.0f) {
                lights[i] = lights.back();
                lights.pop_back();
            }
        }
    }
}

std::vector<PointLight> LightManager::getLights() {
    return lights;
}

void LightManager::removeLight(int x, int y) {
    for (int i = lights.size() - 1; i >= 0; i--) {
        if (lights[i].position.x == x && lights[i].position.y == y) {
            lights[i] = lights.back();
            lights.pop_back();
            break;
        }
    }
}

void LightManager::BFSLight() {

    int dim = calculateMapSize();
    int size = dim * 2 + 1;

    struct Node {
        int x, y;
        float intensity;
        int dx, dy;
    };

    std::queue<Node> q;

    for (auto& light : lights) {

        int lx = light.position.x + dim;
        int ly = light.position.y + dim;

        q.push({ lx, ly, light.intensity, 0, 0 });

        lightMap[lx + ly * size] = std::max(lightMap[lx + ly * size], light.intensity);
    }

    const int dirs[8][2] = {
    {1,0}, {-1,0}, {0,1}, {0,-1},
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

    while (!q.empty()) {
        Node curr = q.front();
        q.pop();

        if (curr.intensity < lightMap[curr.x + curr.y * size]) continue;

        for (auto& d : dirs) {
            int nx = curr.x + d[0];
            int ny = curr.y + d[1];

            int worldX = nx - dim;
            int worldY = ny - dim;

            if (nx < 0 || nx >= size || ny < 0 || ny >= size) {
                continue;
            }

            if (getTileRef(worldX, worldY).blocked) {
                continue;
            }

            float newIntensity = curr.intensity * 0.95f;

            // slight diagonal penalty
            if (d[0] != 0 && d[1] != 0) {
                newIntensity *= 0.99f;
            }

            // turn penalty
            if (curr.dx != 0 || curr.dy != 0) {
                if (d[0] != curr.dx || d[1] != curr.dy) {
                    newIntensity *= 0.8f;
                }
            }

            if (newIntensity < 0.05f) continue;

            if (newIntensity > lightMap[nx + ny * size]) {
                lightMap[nx + ny * size] = newIntensity;
                q.push({ nx, ny, newIntensity, d[0], d[1] });
            }
        }
    }
}