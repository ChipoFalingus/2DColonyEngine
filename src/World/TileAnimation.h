#pragma once

#include "Tile.h"

float animationClock;

struct RGB {
    float r, g, b;
};


RGB HSVtoRGB(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float r, g, b;

    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return { r + m, g + m, b + m };
}

void applyAnimation(Tile& tile, glm::vec3& currentColor, wchar_t& currentDisplayChar) {
    if (tile.anim.type == NONE) return;

    float time = Clock::totalTime;

    switch (tile.anim.type) {
    case BREATHE: {
        float min = 0.2f;
        float max = 1.0f;
        float intensity = min + (max - min) * ((sin(time + tile.animOffset) + 1.0f) / 2.0f);

        currentColor.r = currentColor.r * intensity;
        currentColor.g = currentColor.g * intensity;
        currentColor.b = currentColor.b * intensity;
        break;
    }

    case RAINBOW: {
        float hue = fmod((time * 60.0f) + tile.animOffset * 60.0f, 360.0f);
        RGB rgb = HSVtoRGB(hue, 1.0f, 1.0f);

        currentColor.r = rgb.r;
        currentColor.g = rgb.g;
        currentColor.b = rgb.b;
        break;
    }

    case RED_X: {
        tile.anim.isOtherChar = fmod(time, 1.0f) > 0.5f;

        if (tile.anim.isOtherChar) {
            currentDisplayChar = L'X';
            currentColor = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        break;
    }

    case WHITE_BREATHE: {
        float speed = 2.0f;
        float wave = (sin(time * speed + tile.animOffset) + 1.0f) * 0.5f;

        currentColor.r = currentColor.r + (1.0f - currentColor.r) * wave;
        currentColor.g = currentColor.g + (1.0f - currentColor.g) * wave;
        currentColor.b = currentColor.b + (1.0f - currentColor.b) * wave;
        break;
    }

    case WATER: {
        tile.anim.isOtherChar = fmod(time + tile.animOffset * 64.0f, 64.0f) > 60.0f;

        if (tile.anim.isOtherChar) {
            currentDisplayChar = L'~';
            currentColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        break;
    }

    case FIRE: {
        float animTime = time + tile.animOffset;
        float cyclePosition = fmod(animTime, 0.8f);

        if (cyclePosition < 0.2f) {
            currentDisplayChar = L',';
            currentColor = glm::vec3(1.0f, 0.7f, 0.0f);
        }
        else if (cyclePosition < 0.4f) {
            currentDisplayChar = L')';
            currentColor = glm::vec3(1.0f, 0.7f, 0.0f);
        }
        else if (cyclePosition < 0.6f) {
            currentDisplayChar = L'.';
            currentColor = glm::vec3(1.0f, 0.7f, 0.0f);
        }
        else {
            currentDisplayChar = L'(';
            currentColor = glm::vec3(1.0f, 0.7f, 0.0f);
        }
        break;
    }

    case SPEECH_BUBBLE: {
        tile.anim.isOtherChar = fmod(time + tile.animOffset * 2.0f, 2.0f) > 1.0f;

        if (tile.anim.isOtherChar) {
            currentDisplayChar = L'Q';
            currentColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        break;
    }

    case Z: {
        float cyclePosition = fmod(time + tile.animOffset, 1.5f);

        if (cyclePosition < 0.5f) {
            currentDisplayChar = L'z';
            currentColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else if (cyclePosition < 1.0f) {
            currentDisplayChar = L'Z';
            currentColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        break;
    }

    case GUN_SHOT: {
        currentDisplayChar = L'■';
        currentColor = glm::vec3(1.0f, 1.0f, 0.0f);
        tile.anim.timer += Clock::deltaTime;

        if (tile.anim.timer > 0.05f) {
            tile.anim.timer = 0.0f;
            tile.anim.type = NONE;
        }
        break;
    }
    default:
        break;
    }
}





