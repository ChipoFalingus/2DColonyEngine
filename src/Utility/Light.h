#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <queue>
#include <unordered_map>
#include <utility>

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

class LightManager {
private:
    std::vector<PointLight> lights;
	bool dirty = false;

    std::vector<float> lightMap;

public:
    float getLightMapIndex(int x, int y);
    void initLightMap();
    const std::vector<float>& getLightMap();
    void addLight(const glm::vec2& pos, const glm::vec3& col, float rad, float intensity, float lifetime);
    void clearLights();
    void update(float deltaTime);
    std::vector<PointLight> getLights();
    void removeLight(int x, int y);
    void BFSLight();
};