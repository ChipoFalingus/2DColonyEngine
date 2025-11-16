#pragma once
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>


inline std::vector<std::vector<wchar_t>> txtTo2DArray(const std::string& filePath) {
	std::wifstream file(filePath);
	file.imbue(std::locale("en_US.UTF-8"));

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filePath << std::endl;
		return {};
	}

	std::vector<std::vector<wchar_t>> array;
	std::wstring line;
	while (std::getline(file, line)) {
		if (!line.empty() && line[0] == 0xFEFF) {
			line.erase(0, 1);
		}
		std::vector<wchar_t> row(line.begin(), line.end());
		array.push_back(row);
	}
	return array;
}

class Button {
public:
	int lengthX, lengthY;
	int xOffset, yOffset;
	std::vector<std::wstring> staticButton;
	std::vector<std::wstring> hoveredButton;
	std::vector<std::wstring> clickedButton;

	bool isHovered = false;
	bool isClicked = false;

	bool wasClickedLastFrame = false;

	std::function<void()> onClick;

	Button(int xOffset, int yOffset, 
		const std::vector<std::wstring> staticButton, 
		const std::vector<std::wstring> hoveredButton, 
		const std::vector<std::wstring> clickedButton)
		:xOffset(xOffset), yOffset(yOffset), 
		staticButton(staticButton), 
		hoveredButton(hoveredButton), 
		clickedButton(clickedButton) 
	{
		lengthX = staticButton[0].size();
		lengthY = staticButton.size();
	
	};

	bool checkHover(int mouseX, int mouseY) {
		return (mouseX >= xOffset && mouseX < xOffset + lengthX &&
			mouseY >= yOffset && mouseY < yOffset + lengthY);
	}

	std::vector<std::vector<wchar_t>> drawButton(std::vector<std::vector<wchar_t>> UI) {

		const std::vector<std::wstring>* button;

		if (isClicked) {
			button = &clickedButton;
		}
		else if (isHovered) {
			button = &hoveredButton;
		}
		else {
			button = &staticButton;
		}

		for (int i = 0; i < button->size(); i++) {
			for (int j = 0; j < (*button)[i].size(); j++) {
				if (button->size() + xOffset <= UI[0].size() &&
					button[0].size() + yOffset <= UI.size()) {
					UI[i + yOffset][j + xOffset] = (*button)[i][j];
				}
			
			}
		}
		return UI;
	}

	void update(int mouseX, int mouseY, bool clicked) {
		

		isHovered = checkHover(mouseX, mouseY);

		bool justClicked = clicked && !wasClickedLastFrame;

		if (isHovered && justClicked && onClick) {
			onClick();
		}

		wasClickedLastFrame = clicked;
	}
};


class UI {
public:
	std::vector<std::vector<wchar_t>> UI;
	std::vector<std::vector<wchar_t>> holder;
	std::vector<Button> UIButtons;

	void init(std::string path) {
		holder = txtTo2DArray(path);
	}

	void drawUI() {
		for (int x = 0; x < UI.size(); x++)
			for (int y = 0; y < UI[0].size(); y++)
				UI[x][y] = L'#';

		UI = holder;

		for (auto& i : UIButtons) {
			UI = i.drawButton(UI);
		}
	}
};

extern Button Start;
extern Button Options;
extern Button Exit;

extern Button BuildButton;
extern Button ClearButton;
extern Button PlantButton;

extern Button BuildStockpile;



