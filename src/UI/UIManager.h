#pragma once
#include <algorithm>

#include "UIElements.h"

#define BLANK_CHAR L'@'

class UIManager {
private:
    std::vector<UI> frameStack;
    std::vector<std::vector<wchar_t>> masterUI;

    std::unordered_map<UI, std::unique_ptr<Frame>> allFrames;

    
public:
    std::vector<std::vector<wchar_t>>& getMasterUI() {
        return masterUI;
    }

    std::unordered_map<UI, std::unique_ptr<Frame>>& getAllFrames() {
        return allFrames;
	}

    Frame* getFrame(UI name) {
        auto it = allFrames.find(name);
        if (it != allFrames.end())
            return it->second.get();
        return nullptr;
    }

    bool hasFrame(UI name) {
        return std::find(frameStack.begin(), frameStack.end(), name) != frameStack.end();
    }
   

    void draw() {
        for (auto& row : masterUI) {
            std::fill(row.begin(), row.end(), BLANK_CHAR);
        }

        for (auto& frame : frameStack) {
            auto i = getFrame(frame);
            if (!i) continue;
            i->draw(masterUI);
        }
    }

    void update(int mouseX, int mouseY, bool mouseDown) {
        for (auto& frame : frameStack) {
            auto i = getFrame(frame);
            if (!i) continue;
            i->update(mouseX, mouseY, mouseDown);
        }
    }

    void push(UI name) {
        for (auto& i : frameStack) {
            if (i == name) {
                return;
            }
        }
        frameStack.push_back(name);
    }

    void pop() {
        if (!frameStack.empty()) {
            frameStack.pop_back();
        }

    }

    void remove(UI name) {
        frameStack.erase(
            std::remove(frameStack.begin(), frameStack.end(), name),
            frameStack.end()
        );
	}
    // Swaps two different frames, this only works if both aren't active
    void swapFrame(UI name, UI name2) {
        if (hasFrame(name)) {
			std::replace(frameStack.begin(), frameStack.end(), name, name2);
        }
        else {
            std::replace(frameStack.begin(), frameStack.end(), name2, name);
        }
	}

    void addOrRemoveFrame(const UI name) {
        if (!hasFrame(name)) {
            push(name);
        }
        else {
            deleteFrame(name);
        }
	}

    void deleteFrame(UI name) {
        frameStack.erase(
            std::remove(frameStack.begin(), frameStack.end(), name),
            frameStack.end()
        );
    }

    void addFrame(std::unique_ptr<Frame> frame, UI name) {
        allFrames[name] = std::move(frame);
    }

    void resize(int newX, int newY) {
        masterUI.resize(newY);
        for (auto& row : masterUI) {
            row.resize(newX, L' ');
        }
    }
};