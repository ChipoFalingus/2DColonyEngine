#pragma once

class Clock {

public:
	static inline float deltaTime = 0.0f;
	static inline float totalTime = 0.0f;

	static void update(float dt) {
		deltaTime = dt;
		totalTime += dt;
	}
};