#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <queue>
#include <unordered_map>
#include <utility>

#include "Globals.h"
#include "mathUtils.h"
#include "Tile.h"

// Needs to be optimized and more robust

// Types of lights:
    // Breathing (sine wave)
    // Flickering/Strobe (can used pre-defined patterns)
	// Color changing (takes in a list of colors to cycle through)
    // Directional (vec2 to point at)
	// Static (nothing special)
    // And more



struct Light {
    glm::vec2 position;
    glm::vec3 color;
    float radius;
    float intensity;
	float lifetime;

    Light(glm::vec2 pos, glm::vec3 col, float rad, float intensity, float life)
        : position(pos), color(col), radius(rad),
        intensity(intensity), lifetime(life) {
    }
};

struct PointLight : public Light {
    PointLight(glm::vec2 pos, glm::vec3 col, float rad, float intensity, float life)
        : Light(pos, col, rad, intensity, life) {
    }
};

struct DirectionalLight : public Light {
    glm::vec2 direction;
    DirectionalLight(glm::vec2 pos, glm::vec3 col, float rad, float intensity, float life, glm::vec2 direction)
        : Light(pos, col, rad, intensity, life), direction(direction) {
    }
};

class LightManager {
private:
    std::vector<PointLight> lights;
    std::vector<DirectionalLight> directionLights;

	bool calculateLightLevelDirty = false;

public:
    
    void addLight(const glm::vec2& pos, const glm::vec3& col, float rad, float intensity, float lifetime) {
        lights.emplace_back(pos, col, rad, intensity, lifetime);
    }

    void addDLight(const glm::vec2& pos, const glm::vec3& col, float rad, float intensity, float lifetime, const glm::vec2& direction) {
        directionLights.emplace_back(pos, col, rad, intensity, lifetime, direction);
    }

    void clearLights() {
        lights.clear();
    }

    void update(float deltaTime) {
        for (int i = lights.size() - 1; i >= 0; i--) {
            if (lights[i].lifetime >= 0.0f) {
                lights[i].lifetime -= deltaTime;
                if (lights[i].lifetime <= 0.0f) {
                    lights[i] = lights.back();
                    lights.pop_back();
                }
            }
        }

        for (int i = directionLights.size() - 1; i >= 0; i--) {
            if (directionLights[i].lifetime >= 0.0f) {
                directionLights[i].lifetime -= deltaTime;
                if (directionLights[i].lifetime <= 0.0f) {
                    directionLights[i] = directionLights.back();
                    directionLights.pop_back();
                }
            }
        }

    }

    std::vector<PointLight> getLights() {
		return lights;
    }

    void removeLight(size_t index) {
        if (index < lights.size()) {
            lights.erase(lights.begin() + index);
        }
    }

    glm::vec3 calculateLightLevel(int x, int y) {
        glm::vec3 lightColor(0.1f, 0.1f, 0.1f);

        for (auto& light : lights) {
            float dx = x - light.position.x;
            float dy = y - light.position.y;

            float distSq = dx * dx + dy * dy;
            float falloff = light.intensity / (distSq + 1.0f);

            falloff = std::min(falloff, 1.0f);
            lightColor += light.color * falloff;
        }

        for (auto& light : directionLights) {
            float dx = x - light.position.x;
            float dy = y - light.position.y;

            float distSq = dx * dx + dy * dy;
            float radiusSq = light.radius * light.radius;

            if (distSq > radiusSq) continue;

            glm::vec2 toTile = glm::normalize(glm::vec2(dx, dy));
            glm::vec2 dir = glm::normalize(light.direction);

            float alignment = glm::dot(toTile, dir);

            if (alignment <= 0.0f) continue;

            float falloff = 1.0f - (distSq / radiusSq);
            float directionalStrength = pow(alignment, 16.0f);
            float contribution = falloff * directionalStrength * light.intensity;

            lightColor += light.color * contribution;
        }

        return glm::min(lightColor, glm::vec3(1.0f));
    }
    

    //glm::vec3 calculateLight(int x, int y) {
        // THE PLAN:

        // The Tricky Part:
            // This needs to light up the whole world, but stay fast and optimal
            // I only need to get the visual part correct, the rest can just be updated upon it coming into view

        // Way Around It:
            // Run a BFS search for the whole world once during the loading screen for setup
            // Whenever a new light is added to the scene, only update the chunk it resides in + any it bleeds into (Only if its in view)
            // If I'm desperate for performance, I can offload the BFS search to another thread

        // Possible issues:
            // NPC's will need to know how bright adjacent tiles are, should there be thousands of NPC's scattered, the whole world needs updates constantly

        // Addressing issues:
            // NPC's may not need the actual light level, they just need the location of light sources to guess the brightness


    //}


    // This should update a global light map, it can be a static array because the world size cant change
    std::vector<std::vector<float>> BFSLight(int width, int height) {
        std::vector<std::vector<float>> lightMap(width, std::vector<float>(height, 0.0f));

        struct Node {
            int x, y;
            float intensity;
        };

        std::queue<Node> q;

        for (auto& light : lights) {

            int x = light.position.x;
            int y = light.position.y;

            q.push({ x, y, light.intensity });

            lightMap[x][y] = std::max(lightMap[x][y], light.intensity);
        }

        const int dirs[4][2] = {
        {1,0}, {-1,0}, {0,1}, {0,-1}
        };

        while (!q.empty()) {
            Node curr = q.front();
            q.pop();

            if (curr.intensity < lightMap[curr.x][curr.y]) continue;

            for (auto& d : dirs) {
                int nx = curr.x + d[0];
                int ny = curr.y + d[1];

                if (!getTileRef(nx, ny).walkable) continue;

                float newIntensity = curr.intensity * 0.8f;

                if (newIntensity < 0.01f) continue;

                if (newIntensity > lightMap[nx][ny]) {
                    lightMap[nx][ny] = newIntensity;
                    q.push({ nx, ny, newIntensity });
                }
            }
        }

        return lightMap;
    }
};