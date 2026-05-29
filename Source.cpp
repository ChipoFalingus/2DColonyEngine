#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

//#ifdef _DEBUG
//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <thread>
#include <future>
#include <chrono>

#include "mathUtils.h"
#include "Pair.h"
#include "Tile.h"
#include "Chunk.h"
#include "Item.h"
#include "Gun.h"
#include "Job.h"
#include "Globals.h"
#include "UIElements.h"
#include "Crafting.h"
#include "HarvestRules.h"
#include "World.h"
#include "Food.h"
#include "UIManager.h"
#include "Game.h"
#include "UI.h"
#include "Creature.h"
#include "TileAnimation.h"

#include "Pig.h"
#include "Zombie.h"

#include "Shader.h"
#include "Light.h"
#include "Clock.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Audio.hpp>

#include <ft2build.h>
#include "Spawner.h"
#include FT_FREETYPE_H


void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void setMode(Mode mode);

std::random_device rd;
int seed;

sf::Clock loadTimer;

static std::vector<float> batchVertices; // flattened: x,y,u,v,r,g,b per vertex
static const size_t BATCH_VERTEX_SIZE = 7; // floats per vertex
static const size_t QUAD_VERTS = 6; // two triangles per quad

sf::Font font;
sf::Text text;
std::string textString;

GLuint VAO, VBO, EBO;

struct Character {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
    glm::vec2 UV0;       // top-left in atlas
    glm::vec2 UV1;
};

struct GlyphVertex {
    glm::vec2 pos;   // screen position
    glm::vec2 uv;    // texture coordinates
    glm::vec3 color; // RGB color
};


std::unordered_map<wchar_t, Character> Characters;

GLuint fontTexture;

std::vector<float> vertices;

std::vector<std::pair<int, int>> activeWater;

struct CustomGlyph {
    FT_ULong codepoint;
    int width, height;
    int bearingX, bearingY;
    int advance;
    const unsigned char* bitmap;
};

unsigned char pickaxeBitmap[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 255, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 0, 255, 0, 0, 255, 0,
  0, 0, 0, 0, 0, 0, 0, 255, 255, 0, 255, 255, 0, 0, 255, 0,
  0, 0, 0, 0, 0, 0, 255, 255, 0, 255, 255, 0, 255, 0, 255, 0,
  0, 0, 0, 0, 0, 255, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0,
  0, 0, 0, 0, 255, 255, 0, 255, 255, 0, 0, 0, 255, 0, 255, 0,
  0, 0, 0, 255, 255, 0, 255, 255, 0, 0, 0, 0, 0, 255, 255, 0,
  0, 0, 255, 255, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 255, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 255, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

unsigned char checkboxBitmap[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

unsigned char checkedboxBitmap[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 255, 0,
  0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 255, 0,
  0, 255, 255, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 255, 0,
  0, 255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 0,
  0, 255, 255, 255, 255, 0, 255, 255, 255, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
  0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};



std::unordered_map<FT_ULong, CustomGlyph> customGlyphs = {
    {0x26CF, {0x26CF, 16, 16, 0, 16, 16, pickaxeBitmap}},
    {0xA32, {0xA32, 16, 16, 0, 16, 16, checkboxBitmap}},
    {0xA33, {0xA33, 16, 16, 0, 16, 16, checkedboxBitmap}},
};

sf::Clock day;

void generateFontAtlas(const std::string& fontPath, int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) { std::cerr << "Could not init FreeType\n"; return; }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) { std::cerr << "Failed to load font\n"; return; }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const int atlasWidth = 4096;
    const int atlasHeight = 4096;
    unsigned char* atlasData = new unsigned char[atlasWidth * atlasHeight];
    memset(atlasData, 0, atlasWidth * atlasHeight);

    int x = 0, y = 0, rowHeight = 0;

    for (FT_ULong c = 32; c < 0xFFFF; c++) {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        FT_Bitmap& bmp = face->glyph->bitmap;

        if (x + bmp.width >= atlasWidth) { x = 0; y += rowHeight; rowHeight = 0; }
        if (y + bmp.rows >= atlasHeight) { std::cerr << "Atlas too small!\n"; break; }

        for (int row = 0; row < bmp.rows; row++)
            for (int col = 0; col < bmp.width; col++)
                atlasData[(y + row) * atlasWidth + (x + col)] = bmp.buffer[row * bmp.pitch + col];

        Character character;
        character.Size = glm::ivec2(bmp.width, bmp.rows);
        character.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        character.Advance = face->glyph->advance.x >> 6;
        character.UV0 = glm::vec2((float)x / atlasWidth, (float)y / atlasHeight);
        character.UV1 = glm::vec2((float)(x + bmp.width) / atlasWidth, (float)(y + bmp.rows) / atlasHeight);
        Characters[c] = character;

        x += bmp.width + 1;
        if (bmp.rows > rowHeight) rowHeight = bmp.rows;
    }

    for (const auto& h : customGlyphs) {
		auto& g = h.second;
        if (x + g.width >= atlasWidth) { x = 0; y += rowHeight; rowHeight = 0; }
        if (y + g.height >= atlasHeight) { std::cerr << "Atlas too small for custom glyphs!\n"; break; }

        for (int row = 0; row < g.height; row++)
            for (int col = 0; col < g.width; col++)
                atlasData[(y + row) * atlasWidth + (x + col)] = g.bitmap[row * g.width + col];

        Character character;
        character.Size = glm::ivec2(g.width, g.height);
        character.Bearing = glm::ivec2(g.bearingX, g.bearingY);
        character.Advance = g.advance;
        character.UV0 = glm::vec2((float)x / atlasWidth, (float)y / atlasHeight);
        character.UV1 = glm::vec2((float)(x + g.width) / atlasWidth, (float)(y + g.height) / atlasHeight);
        Characters[g.codepoint] = character;

        x += g.width + 1;
        if (g.height > rowHeight) rowHeight = g.height;
    }

    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] atlasData;
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}



void drawTxtToMap(const std::string& filePath, int x, int y) {
    std::wifstream file(filePath);
    file.imbue(std::locale("en_US.UTF-8"));

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::wstring line;

    int offsetY = 0;
    while (std::getline(file, line)) {
        for (size_t i = 0; i < line.size(); i++) {
            if (!line.empty() && line[0] == 0xFEFF) {
                line.erase(0, 1);
            }
            wchar_t ch = line[i];
            if (ch == L' ') continue;
            int tileX = x + static_cast<int>(i);
            int tileY = y + offsetY;
            Tile& tile = getTileRef(tileX, tileY);
            tile.walkable = false;
        }
        offsetY++;
    }
}

static void ClearBatch() {
    batchVertices.clear();
}

static void FlushBatch(Shader& shader) {
    if (batchVertices.empty()) return;

    float zoom = 1.0f;
    glm::mat4 projection = glm::ortho(0.0f, (float)scrWidth * zoom, 0.0f, (float)scrHeight * zoom);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setInt("text", 0);      // ensure sampler unit 0

    int width = xFrustum;
    int height = yFrustum;
   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glBindVertexArray(VAO);

    // upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, batchVertices.size() * sizeof(float), batchVertices.data(), GL_DYNAMIC_DRAW);

    GLsizei vertexCount = static_cast<GLsizei>(batchVertices.size() / BATCH_VERTEX_SIZE);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    batchVertices.clear();
}


void RenderText(Shader& shader, const wchar_t& text, float x, float y, float scale, glm::vec3 color) {

    // Build quads into batchVertices
    //for (wchar_t c : text) {
        //if (Characters.find(c) == Characters.end()) continue;
        Character ch = Characters[text];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float u0 = ch.UV0.x, v0 = ch.UV0.y;
        float u1 = ch.UV1.x, v1 = ch.UV1.y;

        // six vertices (two triangles). Each vertex: x,y,u,v,r,g,b
        float r = color.r, g = color.g, b = color.b;

        float quad[6][7] = {
            { xpos,     ypos + h,   u0, v0, r, g, b },
            { xpos,     ypos,       u0, v1, r, g, b },
            { xpos + w, ypos,       u1, v1, r, g, b },

            { xpos,     ypos + h,   u0, v0, r, g, b },
            { xpos + w, ypos,       u1, v1, r, g, b },
            { xpos + w, ypos + h,   u1, v0, r, g, b }
        };



        batchVertices.insert(batchVertices.end(), &quad[0][0], &quad[0][0] + 6 * 7);

        x += ch.Advance * scale;
    //}
}

sf::Color hsvToRgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - std::fabs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float r, g, b;
    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255)
    );
}

sf::Color altitudeToColor(float altitude, float minAlt, float maxAlt) {
    // Normalize [0,1]
    float t = (altitude - minAlt) / (maxAlt - minAlt);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    // Map altitude to hue (0–360 degrees)
    float hue = 360.0f * t;

    // Full saturation, full value for vivid rainbow
    return hsvToRgb(hue, 1.0f, 1.0f);
}

void drawMiniMap(Shader& shader) {
    int numX = xPlayer - xFrustum / 2;
    int numY = yPlayer - yFrustum / 2;
    for (int y = yPlayer - yFrustum / 2; y < yPlayer + yFrustum / 2; y++) {
        for (int x = xPlayer - xFrustum / 2; x < xPlayer + xFrustum / 2; x++) {

            /*int chunkX = static_cast<int>(std::floor((float)x));
            int chunkY = static_cast<int>(std::floor((float)y));

            int localX = (x % chunkDim + chunkDim) % chunkDim;
            int localY = (y % chunkDim + chunkDim) % chunkDim;*/

            float screenX = (x - numX) * xTextSpacing;
            float screenY = scrHeight - ((y - numY + 1) * yTextSpacing);

            //Chunk& chunk = mainWorld.loadOrGenerateChunk(chunkX, chunkY);

            Tile* a = nullptr;
            Chunk* chunk = nullptr;

            if (mainWorld.isRendered()) {
                int chunkX = static_cast<int>(std::floor((float)x));
                int chunkY = static_cast<int>(std::floor((float)y));
                chunk = mainWorld.getChunk(chunkX, chunkY);
            }

            wchar_t charStr;
            glm::vec3 color;

            wchar_t string;

            if (viewUI) {
                int uiX = x - (xPlayer - xFrustum / 2);
                int uiY = y - (yPlayer - yFrustum / 2);

                auto& frame = Game::getInstance().getUIManager().getMasterUI();

                if (uiY >= 0 && uiY < frame.size() && uiX >= 0 && uiX < frame[uiY].size()) {
                    wchar_t ch = frame[uiY][uiX];
                    if (ch != L'@') {
                        string = ch;
                        if (ch == L' ') {
                            color = glm::vec3(0.0f, 0.0f, 0.0f);
                        }
                        else {
                            color = glm::vec3(1.0f, 1.0f, 1.0f);
                        }

                        RenderText(shader, string, screenX, screenY, fontSize, color);
                        continue;
                    }
                }
            }

            if (!chunk) {
                RenderText(shader, L'≈', screenX, screenY, fontSize, glm::vec3(0, 0, 1));
                continue;
            }
            
            if (!viewHeightMap) {
                // Convert vector<wchar_t> to wstring(use first character if available)
                charStr = chunk->dominantDisplay.chars.empty() ? L' ' : chunk->dominantDisplay.chars[0];
                // Convert vector<sf::Color> to glm::vec3 (use first color if available)
                color = chunk->dominantDisplay.colors.empty() ? glm::vec3(1.0f, 1.0f, 1.0f) :
                    glm::vec3(
                        chunk->dominantDisplay.colors[0].r / 255.0f,
                        chunk->dominantDisplay.colors[0].g / 255.0f,
                        chunk->dominantDisplay.colors[0].b / 255.0f
                    );
            }
            else {
                // Convert vector<wchar_t> to wstring(use first character if available)
                charStr = L'■';
                // Convert vector<sf::Color> to glm::vec3 (use first color if available)
                color = chunk->dominantDisplay.colors.empty() ? glm::vec3(1.0f, 1.0f, 1.0f) :
                    glm::vec3(
                        altitudeToColor(chunk->avgHeight, -100.0f, 100.0f).r / 255.0f,
                        altitudeToColor(chunk->avgHeight, -100.0f, 100.0f).g / 255.0f,
                        altitudeToColor(chunk->avgHeight, -100.0f, 100.0f).b / 255.0f
                    );
            }
            
            RenderText(shader, charStr, screenX, screenY, fontSize, color);
        }
    }
}

sf::Color regionColor(int region) {
    // deterministic pseudo-random based on region id
    unsigned int x = static_cast<unsigned int>(region);

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    // extract RGB components
    sf::Uint8 r = 80 + (x & 0x7F);         x >>= 8;
    sf::Uint8 g = 80 + (x & 0x7F);         x >>= 8;
    sf::Uint8 b = 80 + (x & 0x7F);

    return sf::Color(r, g, b);
}

void drawMap(Shader& shader)
{
    int numX = xPlayer - xFrustum / 2;
    int numY = yPlayer - yFrustum / 2;

    std::unordered_set<std::pair<int, int>, pair_hash> lineTiles;

    if (mainWorld.placementMode == PlacementMode::LINE && placing) {
        auto line = bresenham(corner.first, corner.second, mouseTileX, mouseTileY);
		lineTiles.insert(line.begin(), line.end());
    }


    std::unordered_map<std::pair<int, int>, Creature*, pair_hash> creaturesInScope;


    for (auto& creature : mainWorld.getAllCreatures()) {
        if (!creature) continue;
        int cx = creature->xPos;
        int cy = creature->yPos;
        if (cx >= xPlayer - xFrustum / 2 && cx <= xPlayer + xFrustum / 2 &&
            cy >= yPlayer - yFrustum / 2 && cy <= yPlayer + yFrustum / 2) {
            creaturesInScope[{cx, cy}] = creature.get();
        }
    }

    for (int y = yPlayer - yFrustum / 2; y < (yPlayer + yFrustum / 2) + 1; y++) {

        float screenY = scrHeight - ((y - numY + 1) * yTextSpacing);
        for (int x = xPlayer - xFrustum / 2; x < (xPlayer + xFrustum / 2) + 1; x++) {

            float screenX = (x - numX) * xTextSpacing;
            Tile* a = nullptr;
            Chunk* chunk = nullptr;
            if (mainWorld.isRendered()) {
                int chunkX = x / chunkDim;
                int chunkY = y / chunkDim;
                chunk = mainWorld.getChunk(chunkX, chunkY);

                if (chunk) {
                    a = &getTileRef(x, y);
                    a->update();
                }
            }

            wchar_t string;
            glm::vec3 color(1.0f, 1.0f, 1.0f); // default white

            if (viewUI) {
                int uiX = x - (xPlayer - xFrustum / 2);
                int uiY = y - (yPlayer - yFrustum / 2);

                auto& frame = Game::getInstance().getUIManager().getMasterUI();

                if (uiY >= 0 && uiY < frame.size() && uiX >= 0 && uiX < frame[uiY].size()) {
                    wchar_t ch = frame[uiY][uiX];
                    if (ch != L'@') {
                        //string = std::wstring(1, ch);
						string = ch;
                        if (ch == L' ') {
                            color = glm::vec3(0.0f, 0.0f, 0.0f);
                        }
                        else {
                            color = glm::vec3(1.0f, 1.0f, 1.0f);
                        }

                        RenderText(shader, string, screenX, screenY, fontSize, color);
                        continue;
                    }
                }
            }

            if (!mainWorld.isRendered() || !chunk) {
                continue;
            }

            Tile& tile = getTileRef(x, y);

            // Tile contents
            if (tile.items.size() != 0) {
                auto display = tile.items[0]->getVisual();
                if (tile.items[0]->type == Type::Tool) {
                    auto* tool = static_cast<Tool*>(tile.items[0].get());
                    color = materialToColor(tool->material);
                }
                else {
                    color = glm::vec3(display.displayColor.r / 255.0f, display.displayColor.g / 255.0f, display.displayColor.b / 255.0f);
                }


                if (tile.anim.type != animType::NONE) {
                    sf::Color d = getColor(tile);
                    color = glm::vec3(d.r / 255.0f, d.g / 255.0f, d.b / 255.0f);
                }

                string = display.displayChar;
            }
            else {
                string = tile.character;
                color = glm::vec3(tile.color.r / 255.0f, tile.color.g / 255.0f, tile.color.b / 255.0f);
            }
        

            if (placing) {
                if (mainWorld.placementMode == PlacementMode::SQUARE) {
                    int left = std::min(corner.first, mouseTileX);
                    int right = std::max(corner.first, mouseTileX);
                    int top = std::min(corner.second, mouseTileY);
                    int bottom = std::max(corner.second, mouseTileY);

                    if (x >= left && x <= right && y >= top && y <= bottom) {
                        if (mainWorld.atStockpile(x, y) || !tile.walkable) {
                            string = L'X';
                            color = glm::vec3(1.0f, 0.0f, 0.0f);
                        }
                        else {
                            if (y == top || y == bottom || x == left || x == right) {
                                string = L'+';
                                color = glm::vec3(1.0f, 0.0f, 0.0f);
                            }
                        }
                    }
                }
                if (mainWorld.placementMode == PlacementMode::LINE) {
                    if (lineTiles.find(std::make_pair(x, y)) != lineTiles.end()) {
                        string = L'+';
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                }
                if (mainWorld.placementMode == PlacementMode::SINGLE) {
                    if (x == mouseTileX && y == mouseTileY) {
                        string = L'X';
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                }
            }

			auto it = creaturesInScope.find(std::make_pair(x, y));
            if (it != creaturesInScope.end()) {
                string = it->second->displayChar;
                color = it->second->displayColor;

				color.r /= 255.0f;
				color.g /= 255.0f;
				color.b /= 255.0f;
			}

            if (viewHeightMap) {
                string = L'■';
                color =
                    glm::vec3(
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).r / 255.0f,
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).g / 255.0f,
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).b / 255.0f
                    );
            }

            int size = calculateMapSize();
            int lx = x + size;
            int ly = y + size;

            float dayLength = 1200.0f;
            float pi = 3.14159f;

            float t = day.getElapsedTime().asSeconds();
            float cycle = t * (2.0f * pi / dayLength);

            float time = 0.5f * sin(cycle - pi / 2.0f) + 0.5f;

            float ambient = std::max(time, 1.0f);
            //float ambient = 0.2f;
            color *= std::max(mainWorld.getLightMapIndex(x, y), ambient);
            if (string != L'\0') {
                RenderText(shader, string, screenX, screenY, fontSize, color);
            }
            else {
				RenderText(shader, L' ', screenX, screenY, fontSize, glm::vec3(1.0f));
            }
        }
        //RenderText(shader, line, 0.0f, screenY, fontSize, glm::vec3(1.0f), false);
    };
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	scrWidth = 1920;
	scrHeight = 1080;

    //scrWidth = mode->width;
    //scrHeight = mode->height;

	xFrustum = scrWidth / xTextSpacing;
    yFrustum = scrHeight / yTextSpacing;


    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "ASCII Game", NULL, NULL);
    //glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, scrWidth, scrHeight);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    generateFontAtlas("cour.ttf", 48 * 0.55);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, BATCH_VERTEX_SIZE * sizeof(float), (void*)(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, BATCH_VERTEX_SIZE * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, BATCH_VERTEX_SIZE * sizeof(float), (void*)(4 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader shader("shader.vert", "shader.frag");

    std::random_device rd;
    seed = rd();
    rng.seed(seed);
    srand(seed);

    // Item setup
    loadObjects();
	loadHarvestRules();
    loadRecipes();

    Game::getInstance().initUI();

    auto& uiManager = Game::getInstance().getUIManager();
	uiManager.resize(xFrustum, yFrustum);
    uiManager.push(UI::Main);
	
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    std::string fps;

    sf::Music music;

    if (!music.openFromFile("menumusic.mp3")) {
        std::cout << "Music failed to load!" << std::endl;
    }

    //music.play();

    framebuffer_size_callback(window, scrWidth, scrHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSwapInterval(0);

    float lightClock = 0.0f;
	float waitingUpdateTimer = 0.0f;

    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        Clock::update(dt * speed);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ClearBatch();

        // Draw text
        if (viewMiniMap) {
            drawMiniMap(shader);
        }
        else {
            drawMap(shader);
        }

		FlushBatch(shader);

        // Inputs
        processInput(window);

        Game::getInstance().getLightManager().update(dt);

        lightClock += dt;
        if (lightClock > .1f) {
			lightClock = 0.0f;
            std::vector<float> map = Game::getInstance().getLightManager().BFSLight();
            mainWorld.setLightMap(map);
        }

        // Update creatures
        // Move this to the World class later
        auto& creatures = mainWorld.getAllCreatures();

        creatures.erase(
            std::remove_if(creatures.begin(), creatures.end(),
                [](const std::unique_ptr<Creature>& c) {
                    return c->dead;
                }),
            creatures.end()
        );

        for (auto& c : creatures) {
            if (!c) continue;
            c->doWork();
        }

        glfwGetCursorPos(window, &mouseX, &mouseY);

        mouseTileX = (int)(mouseX / xTextSpacing) + xPlayer - (scrWidth / (2 * xTextSpacing));
        mouseTileY = (int)(mouseY / yTextSpacing) + yPlayer - (scrHeight / (2 * yTextSpacing));

        if (viewUI) {
            uiManager.update(mouseX / xTextSpacing, mouseY / yTextSpacing, clicked);
            uiManager.draw();
        }

        if (mainWorld.isCurrentlyRendering()) {
            LoadingUI& ui = Game::getInstance().getLoadingUI();
            int numChunks = (calculateMapSize() * calculateMapSize()) * 4 / (chunkDim * chunkDim);

            if (numChunks > mainWorld.getChunksRendered()) {
                ui.chunks->changeText(std::to_wstring(mainWorld.getChunksRendered()) + L"/" + std::to_wstring(numChunks));
            }
            else {
                ui.chunks->changeText(L"Finishing things up...");
            }


            int barLength = xFrustum - 2;

            float percent = std::clamp((float)mainWorld.getChunksRendered() / numChunks, 0.0f, 1.0f);
            int filled = (int)(percent * barLength);
            
            std::wstring bar;
            bar += std::wstring(filled, L'#');
            bar += std::wstring(barLength - filled, L'-');

            ui.animation->changeText(bar);
        }

        if (mainWorld.isRendered()) {

            squad1.update();

            // All dynamic tiles need to be added to this list
            for (auto& tile : tiles) {
                auto i = getTileRef(tile.first, tile.second).items[0];
                if (i->type == Type::Spawner) {
                    auto j = static_cast<Spawner*>(i.get());
                    j->update();
                }
            }

            JobManager::update();

            // Stockpile item moving

            // Move this to the Colony class later
            auto& itemsToMove = mainWorld.getItemsToMove();

            for (auto it = itemsToMove.begin(); it != itemsToMove.end(); ) {
                auto& item = it->first;

                auto spotOpt = mainWorld.findStockpileSpotForItem(item->name, it->second.first, it->second.second);

                if (spotOpt) {
                    auto [stockpile, pos] = *spotOpt;
                    stockpile->addItem(item, pos.first, pos.second);

                    Job* job = new MoveItem(nullptr, nullptr, SkillType::None, item, it->second.first, it->second.second, pos.first, pos.second);

                    job->priority = 5;
                    JobManager::addJob(job);
                    it = itemsToMove.erase(it);
                }
                else {
                    it++;
                }
            }
        
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	scrWidth = width;
	scrHeight = height;
    xFrustum = scrWidth / xTextSpacing;
    yFrustum = scrHeight / yTextSpacing;
    std::cout << "Tile dimesions resized to " << xFrustum << "x" << yFrustum << std::endl;

	auto& uiManager = Game::getInstance().getUIManager();
	uiManager.resize(xFrustum, yFrustum);
    for (auto& i : uiManager.getAllFrames()) {
		auto frame = i.second.get();
        frame->resize(xFrustum, yFrustum);

    }
}
