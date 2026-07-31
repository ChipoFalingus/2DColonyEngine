#pragma once
#include <memory>
#include <mutex>

struct Settings {

	int xTextSpacing = 16;
	int yTextSpacing = 22;

	int camera_speed = 100;

	float font_size = 1.0f;

	bool v_sync = false;
	bool show_fps = false;

};

class SettingsManager {
private:
	Settings m_settings;
	mutable std::mutex mtx;
public:

	Settings get() const {
		std::scoped_lock lock(mtx);
		return m_settings;
	}

	void update(Settings& newData) {
		std::scoped_lock lock(mtx);
		m_settings = newData;
	}
};