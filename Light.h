#pragma once
#include <vector>
#include <glm/glm.hpp>


struct Light {
    glm::vec2 position;
    glm::vec3 color;
    float radius;
    float additionalIntensity;
	float lifetime;

    Light(glm::vec2 pos, glm::vec3 col, float rad, float intensity, float life)
        : position(pos), color(col), radius(rad),
        additionalIntensity(intensity), lifetime(life) {
    }
};



class LightManager {

public:
    static std::vector<Light> lights;

    static void addLight(const glm::vec2& pos, const glm::vec3& col, float rad, float intensity, float lifetime) {
        lights.emplace_back(pos, col, rad, intensity, lifetime);
    }

    static void clearLights() {
        lights.clear();
    }

    static void update(float deltaTime) {
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


    static void removeLight(size_t index) {
        if (index < lights.size()) {
            lights.erase(lights.begin() + index);
        }
    }
    
};


