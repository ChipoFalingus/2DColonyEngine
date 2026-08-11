#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <queue>
#include <unordered_map>
#include <utility>

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

    void removeLight(int x, int y) {
        for (int i = lights.size() - 1; i >= 0; i--) {
            if (lights[i].position.x == x && lights[i].position.y == y) {
                lights[i] = lights.back();
                lights.pop_back();
                break;
            }
        }
    }

    /*glm::vec3 calculateLightLevel(int x, int y) {
        glm::vec3 lightColor(0.1f);

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
    }*/


    // This should update a global light map
    std::vector<float> BFSLight() {

        int dim = calculateMapSize();
        int size = dim * 2 + 1;

        std::vector<float> lightMap(size * size, 0.0f);

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
                    q.push({ nx, ny, newIntensity, d[0], d[1]});
                }
            }
        }

        return lightMap;
    }
};