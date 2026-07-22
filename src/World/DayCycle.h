#pragma once
#include "World/World.h"
#include "Utility/Clock.h"

enum class TimePeriod {
    Morning,
    Afternoon,
    Evening,
    Night
};

inline std::string timePeriodToString(TimePeriod period) {
    switch (period) {
    case TimePeriod::Morning: return "Morning";
    case TimePeriod::Afternoon: return "Afternoon";
    case TimePeriod::Evening: return "Evening";
    case TimePeriod::Night: return "Night";
    }
    return "Unknown";
}

class DayCycle {
private:
    float dayLength = 1200.0f;

	// Start at morning
    float currentTime = dayLength * 0.33f;
    int dayNumber = 1;

public:

    void update(float gameSpeedMultiplier = 1.0f) {
        currentTime += Clock::deltaTime * gameSpeedMultiplier;

        if (currentTime >= dayLength) {
            currentTime -= dayLength;
            dayNumber++;
        }
    }

    float getDaylightFactor() const {
        float pi = 3.14159265f;
        float cycle = currentTime * (2.0f * pi / dayLength);

        //return std::max(0.5f * std::sin(cycle - pi / 2.0f) + 0.55f, 0.1f);
        return 1.1f;
    }

    float getTemperatureFactor() const {
        float pi = 3.14159265f;
        float cycle = currentTime * (2.0f * pi / dayLength);
        return std::max(15.f * std::sin(cycle - pi / 2.0f) + 65.f, 0.1f);
	}

    TimePeriod getTimePeriod() const {
        float progress = currentTime / dayLength;

        if (progress < 0.15f) return TimePeriod::Night;
        if (progress < 0.40f) return TimePeriod::Morning;
        if (progress < 0.75f) return TimePeriod::Afternoon;
        if (progress < 0.90f) return TimePeriod::Evening;
        return TimePeriod::Night;   
    }

    int getDayNumber() const { return dayNumber; }
    float getRawTime() const { return currentTime; }
};