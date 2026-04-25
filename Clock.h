#pragma once




class Clock {

public:
	float deltaTime = 0.0f;
	float totalTime = 0.0f;

	void update(float dt) {
		deltaTime = dt;
		totalTime += dt;
	}
};