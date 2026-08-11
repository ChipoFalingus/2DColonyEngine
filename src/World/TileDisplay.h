#pragma once
#include <vector>
#include <glm/glm.hpp>

struct tileDisplay {
    wchar_t character;
    glm::vec3 color;

	tileDisplay(wchar_t character, glm::vec3 color) : character(character), color(color) {}
};