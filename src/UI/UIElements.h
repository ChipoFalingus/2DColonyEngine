#pragma once
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>

#include "Utility/Globals.h"
#include "UI/UITypes.h"

#define BLANK_CHAR L'@'

enum Anchor {
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	CENTER_LEFT,
	CENTER,
	CENTER_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};

// Good for templates
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
		// 0xFEFF is the BOM character for UTF-8
		if (!line.empty() && line[0] == 0xFEFF) {
			line.erase(0, 1);
		}
		std::vector<wchar_t> row(line.begin(), line.end());
		array.push_back(row);
	}
	return array;
}

class UIElement {
protected:
	int lengthX, lengthY;
	int xOffset, yOffset;
	int xPos, yPos;
	Anchor anchor;
public:
	virtual ~UIElement() = default;
	virtual void draw(std::vector<std::vector<wchar_t>>& buffer) = 0;
	virtual void update(int mouseX, int mouseY, bool mouseDown) {}

	void setPosition(int xOffset, int yOffset) {
		this->xOffset = xOffset;
		this->yOffset = yOffset;
	}

	void setSize(int lengthX, int lengthY) {
		this->lengthX = lengthX;
		this->lengthY = lengthY;
	}

	int getXOffset() const { return xOffset; }
	int getYOffset() const { return yOffset; }

	int getXLength() const { return lengthX; }
	int getYLength() const { return lengthY; }

	int getXPos() const { return xPos; }
	int getYPos() const { return yPos; }

	Anchor getAlignment() const { return anchor; }

	std::pair<int, int> getAnchorPosition(int scrLength, int scrHeight) const {
		int x = 0;
		int y = 0;

		switch (anchor)
		{
		case Anchor::TOP_LEFT:
			break;

		case Anchor::TOP_CENTER:
			x = scrLength / 2 - lengthX / 2;
			break;

		case Anchor::TOP_RIGHT:
			x = scrLength - lengthX;
			break;

		case Anchor::CENTER_LEFT:
			y = scrHeight / 2 - lengthY / 2;
			break;

		case Anchor::CENTER:
			x = scrLength / 2 - lengthX / 2;
			y = scrHeight / 2 - lengthY / 2;
			break;

		case Anchor::CENTER_RIGHT:
			x = scrLength - lengthX;
			y = scrHeight / 2 - lengthY / 2;
			break;

		case Anchor::BOTTOM_LEFT:
			y = scrHeight - lengthY;
			break;

		case Anchor::BOTTOM_CENTER:
			x = scrLength / 2 - lengthX / 2;
			y = scrHeight - lengthY;
			break;

		case Anchor::BOTTOM_RIGHT:
			x = scrLength - lengthX;
			y = scrHeight - lengthY;
			break;
		}

		x += xOffset;
		y += yOffset;

		return { x, y };
	}

	void setAnchorPosition(int l, int h) {
		auto pair = getAnchorPosition(l, h);
		xPos = pair.first;
		yPos = pair.second;
	}
};

// Entire screen UI frame
class Frame {
private:
	std::vector<std::unique_ptr<UIElement>> elements;
	UI type;
public:
	template<typename T, typename... Args>
	T& addElement(Args&&... args) {
		elements.push_back(std::make_unique<T>(std::forward<Args>(args)...));
		return *static_cast<T*>(elements.back().get());
	}

	void removeElement(UIElement* element) {
		elements.erase(std::remove_if(elements.begin(), elements.end(),
			[element](const std::unique_ptr<UIElement>& ptr) { return ptr.get() == element; }),
			elements.end());
	}

	UI getName() const {
		return type;
	}

	void setType(UI type) {
		this->type = type;
	}
	
	void draw(std::vector<std::vector<wchar_t>>& UI) {
		for (auto& i : elements) {
			i->draw(UI);
		}
	}

	void update(int mouseX, int mouseY, bool mouseDown) {
		if (elements.empty()) return;
		for (auto& element : elements) {
			element->update(mouseX, mouseY, mouseDown);
		}
	}

	void resize(int scrLength, int scrHeight) {
		for (auto& element : elements) {
			element->setAnchorPosition(scrLength, scrHeight);
		}
	}
};

class Panel : public UIElement {
private:
	std::vector<std::unique_ptr<UIElement>> elements;
public:
	Panel(int xOffset, int yOffset, int lengthX, int lengthY, Anchor anchor)
		: UIElement()
	{
		this->anchor = anchor;
		setPosition(xOffset, yOffset);
		setSize(lengthX, lengthY);
	}

	template<typename T, typename... Args>
	T& addElement(Args&&... args) {
		auto element = std::make_unique<T>(std::forward<Args>(args)...);
		element->setPosition(
			element->getXOffset() + xOffset,
			element->getYOffset() + yOffset
		);
		element->setAnchorPosition(lengthX, lengthY);

		elements.push_back(std::move(element));
		return *static_cast<T*>(elements.back().get());
	}

	void clear() {
		elements.clear();
	}

	void draw(std::vector<std::vector<wchar_t>>& UI) override {
		for (int i = 0; i < lengthY; i++) {
			for (int j = 0; j < lengthX; j++) {
				if (yPos + i < UI.size() && xPos + j < UI[0].size()) {
					UI[yPos + i][xPos + j] = L' ';

					// Edges
					if (i == 0 || i == lengthY - 1) UI[yPos + i][xPos + j] = L'─';
					if (j == 0 || j == lengthX - 1) UI[yPos + i][xPos + j] = L'│';

					// Corners
					if (i == 0 && j == 0) UI[yPos + i][xPos + j] = L'┌';
					if (i == lengthY - 1 && j == 0) UI[yPos + i][xPos + j] = L'└';
					if (i == 0 && j == lengthX - 1) UI[yPos + i][xPos + j] = L'┐';
					if (i == lengthY - 1 && j == lengthX - 1) UI[yPos + i][xPos + j] = L'┘';
				}
			}
		}
		
		
		for (auto& element : elements) {
			element->draw(UI);
		}
	}
};

class Slider : public UIElement {
private:
	int minValue, maxValue;
	int currentValue;
	int segments;
	bool vertical;
public:
	Slider(int xOffset, int yOffset, int minValue, int maxValue, int currentValue, int segments, bool vertical, Anchor alignment)
		: minValue(minValue), maxValue(maxValue), currentValue(currentValue), segments(segments), vertical(vertical)
	{
		this->anchor = anchor;
		setPosition(xOffset, yOffset);
	}

	bool checkHover(int mouseX, int mouseY) {
		if (vertical) {
			return (mouseX >= xOffset && mouseX < xOffset + 1
				&& mouseY >= yOffset && mouseY < yOffset + segments);
		}
		else {
			return (mouseX >= xOffset && mouseX < xOffset + segments
				&& mouseY >= yOffset && mouseY < yOffset + 1);
		}

	}

	void draw(std::vector<std::vector<wchar_t>>& UI) override {
		float t = float(currentValue - minValue) / float(maxValue - minValue);
		int knobIndex = static_cast<int>(std::round(t * (segments - 1)));

		for (int i = 0; i < segments; i++) {
			int drawX = xOffset + (vertical ? 0 : i);
			int drawY = yOffset + (vertical ? i : 0);

			if (drawY < 0 || drawY >= UI.size() ||
				drawX < 0 || drawX >= UI[drawY].size())
				continue;

			UI[drawY][drawX] = (i == knobIndex) ? L'█' : (vertical ? L'│' : L'─');
		}
	}

	void update(int mouseX, int mouseY, bool mouseDown) {
		if (!mouseDown) return;
		if (!checkHover(mouseX, mouseY)) return;

		int pos = vertical ? (mouseY - yOffset) : (mouseX - xOffset);
		if (pos > segments) pos = segments;
		if (pos < 0) pos = 0;

		float t = float(pos) / float(segments - 1);
		int value = static_cast<int>(std::round(minValue + t * (maxValue - minValue)));

		setValue(value);
	}

	void setValue(int value) {
		if (value < minValue) currentValue = minValue;
		else if (value > maxValue) currentValue = maxValue;
		else currentValue = value;
	}

	int getValue() const {
		return currentValue;
	}
};

class Text : public UIElement {
private:
	std::wstring label;
public:
	Text(int xOffset, int yOffset, const std::wstring& label, Anchor anchor)
		:UIElement(), label(label) 
	{
		this->anchor = anchor;
		setPosition(xOffset, yOffset);
		calculateSize();
	}

	void calculateSize() {
		int maxLineLength = 0;
		int num = 0;
		int lineCount = 1;

		for (wchar_t ch : label) {
			if (ch == L'|') {
				maxLineLength = std::max(maxLineLength, num);
				num = 0;
				lineCount++;
			}
			else {
				num++;
			}
		}

		maxLineLength = std::max(maxLineLength, num);
		setSize(maxLineLength, lineCount);
	}

	void draw(std::vector<std::vector<wchar_t>>& UI) override {
		int line = 0;
		int column = 0;

		if (UI.empty() || UI[0].empty()) return;

		for (size_t idx = 0; idx < label.size(); ++idx) {
			wchar_t ch = label[idx];

			if (ch == L'|') {
				line++;
				column = 0;
				continue;
			}

			int drawY = yPos + line;
			int drawX = xPos + column;

			if (drawY < 0 || drawY >= static_cast<int>(UI.size())) {
				continue;
			}
			if (drawX < 0 || drawX >= static_cast<int>(UI[drawY].size())) {
				continue;
			}

			UI[drawY][drawX] = ch;
			column++;
		}
	}

	void changeText(const std::wstring& newText) {
		label = newText;
	}
};

class Button : public UIElement {
private:
	std::vector<std::wstring> staticButton;
	std::vector<std::wstring> hoveredButton;
	std::vector<std::wstring> clickedButton;

	bool isHovered = false;
	bool isClicked = false;

	bool wasClickedLastFrame = false;

	std::function<void()> onClick;
	std::function<void()> onHover;

public:
	Button(int xOffset, int yOffset, 
		const std::vector<std::wstring> staticButton, 
		const std::vector<std::wstring> hoveredButton, 
		const std::vector<std::wstring> clickedButton,
		Anchor alignment)
		:staticButton(staticButton), 
		hoveredButton(hoveredButton), 
		clickedButton(clickedButton)
	{
		this->anchor = alignment;
		setPosition(xOffset, yOffset);
		setSize(staticButton[0].size(), staticButton.size());
	};

	bool checkHover(int mouseX, int mouseY) { 
		/*return (mouseX >= xOffset && mouseX < xOffset + staticButton[0].size()
			&& mouseY >= yOffset && mouseY < yOffset + staticButton.size());*/
		return (mouseX >= xPos && mouseX < xPos + staticButton[0].size()
			&& mouseY >= yPos && mouseY < yPos + staticButton.size());
		
	}

	void draw(std::vector<std::vector<wchar_t>>& UI) override {
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
				int drawX = xPos + j;
				int drawY = yPos + i;

				if (drawY >= 0 && drawY < UI.size() &&
					drawX >= 0 && drawX < UI[drawY].size()) {
					UI[drawY][drawX] = (*button)[i][j];
				}
			}
		}
	}


	void update(int mouseX, int mouseY, bool mouseDown) {
		isHovered = checkHover(mouseX, mouseY);

		bool justPressed = mouseDown && !wasClickedLastFrame;
		bool justReleased = !mouseDown && wasClickedLastFrame;

		if (isHovered && justPressed) {
			isClicked = true;
		}

		if (isHovered && onHover) {
			onHover();
		}

		if (isClicked && justReleased) {
			if (isHovered) {
				if (onClick) {
					onClick();
				}
			}
			
			isClicked = false;
		}

		wasClickedLastFrame = mouseDown;
	}

	void setClickFunction(std::function<void()> func) {
		onClick = func;
	}

	void setHoverFunction(std::function<void()> func) {
		onHover = func;
	}
};

class Checkbox : public UIElement {
private:
	bool isChecked = false;
	bool isClicked = false;
	bool wasClickedLastFrame = false;
public:

	Checkbox(int xOffset, int yOffset, Anchor anchor)
	{
		this->anchor = anchor;
		setPosition(xOffset, yOffset);
		setSize(1, 1);
	}

	bool getChecked() const {
		return isChecked;
	}

	void setChecked(bool checked) {
		isChecked = checked;
	}

	bool checkHover(int mouseX, int mouseY) {
		return (mouseX >= xOffset && mouseX < xOffset + 1
			&& mouseY >= yOffset && mouseY < yOffset + 1);

	}

	void draw(std::vector<std::vector<wchar_t>>& UI) override {
		if (yOffset >= 0 && yOffset < UI.size() &&
			xOffset >= 0 && xOffset < UI[yOffset].size()) {
			UI[yOffset][xOffset] = isChecked ? 2611 : 2610;
		}
	}

	void update(int mouseX, int mouseY, bool mouseDown) override {
		bool isHovered = checkHover(mouseX, mouseY);

		bool justPressed = mouseDown && !wasClickedLastFrame;
		bool justReleased = !mouseDown && wasClickedLastFrame;

		if (isHovered && justPressed) {
			isClicked = true;
		}

		if (isClicked && justReleased) {
			if (isHovered) {
				isChecked = !isChecked;
			}
			isClicked = false;
		}

		wasClickedLastFrame = mouseDown;
	}

};

// Used for logo
class CustomElement : public UIElement {
private:
	std::vector<std::wstring> content;
public:
	CustomElement(int xOffset, int yOffset, const std::vector<std::wstring>& content)
		:content(content) 
	{
		setPosition(xOffset, yOffset);
		setSize(content[0].size(), content.size());
	}

	void setContent(const std::vector<std::wstring>& newContent) {
		content = newContent;
		setSize(content[0].size(), content.size());
	}

	void draw(std::vector<std::vector<wchar_t>>& UI) override {
		for (int i = 0; i < content.size(); i++) {
			for (int j = 0; j < content[i].size(); j++) {
				if (content.size() + xOffset <= UI[0].size() &&
					content[0].size() + yOffset <= UI.size()) {
					UI[i + yOffset][j + xOffset] = content[i][j];
				}
			}
		}
	}
};
