#pragma once
#include <unordered_set>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Render/Shader.h"
#include "Game.h"
#include "GameState.h"
#include "Settings.h"
#include "World/TileAnimation.h"

#include <ft2build.h>
#include FT_FREETYPE_H

// rendering stuff is finally out of source.cpp

static std::vector<float> batchVertices; // flattened: x,y,u,v,r,g,b per vertex
static const size_t BATCH_VERTEX_SIZE = 7; // floats per vertex
static const size_t QUAD_VERTS = 6; // two triangles per quad

std::unordered_map<wchar_t, Character> Characters;

GLuint fontTexture;
GLuint VAO, VBO, EBO;

struct CustomGlyph {
    FT_ULong codepoint;
    int width, height;
    int bearingX, bearingY;
    int advance;
    const unsigned char* bitmap;
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
    {0xA32, {0xA32, 16, 16, 0, 16, 16, checkboxBitmap}},
    {0xA33, {0xA33, 16, 16, 0, 16, 16, checkedboxBitmap}},
};


struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
    glm::vec2 UV0;
    glm::vec2 UV1;
};

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
        if (y + bmp.rows >= atlasHeight) { std::cerr << "Atlas too small\n"; break; }

        for (int row = 0; row < bmp.rows; row++) {
            for (int col = 0; col < bmp.width; col++) {
                atlasData[(y + row) * atlasWidth + (x + col)] = bmp.buffer[row * bmp.pitch + col];
            }
        }

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
        if (y + g.height >= atlasHeight) { std::cerr << "Atlas too small for custom glyphs\n"; break; }

        for (int row = 0; row < g.height; row++) {
            for (int col = 0; col < g.width; col++) {
                atlasData[(y + row) * atlasWidth + (x + col)] = g.bitmap[row * g.width + col];
            }
        }

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

// probably belongs in MathUtils
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
    float t = (altitude - minAlt) / (maxAlt - minAlt);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    float hue = 360.0f * t;

    return hsvToRgb(hue, 1.0f, 1.0f);
}


void RenderText(Shader& shader, const wchar_t& text, float x, float y, float scale, glm::vec3 color) {
    Character ch = Characters[text];

    float xpos = x + ch.Bearing.x * scale;
    float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;

    float u0 = ch.UV0.x, v0 = ch.UV0.y;
    float u1 = ch.UV1.x, v1 = ch.UV1.y;

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
}


static void ClearBatch() {
    batchVertices.clear();
}

static void FlushBatch(Shader& shader) {
    if (batchVertices.empty()) return;

    GameState& gameState = Game::getInstance().gameState;

    float zoom = 1.0f;
    glm::mat4 projection = glm::ortho(0.0f, (float)gameState.cameraState.scrWidth * zoom, 0.0f, (float)gameState.cameraState.scrHeight * zoom);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setInt("text", 0);

    int width = gameState.cameraState.xFrustum;
    int height = gameState.cameraState.yFrustum;

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


void drawMap(Shader& shader, const Settings& settings, World& world) {
    GameState& gameState = Game::getInstance().gameState;

    int xFrustum = gameState.cameraState.xFrustum;
    int yFrustum = gameState.cameraState.yFrustum;

    int xPlayer = gameState.cameraState.xPlayer;
    int yPlayer = gameState.cameraState.yPlayer;

    int mouseTileX = gameState.inputState.mouseTileX;
    int mouseTileY = gameState.inputState.mouseTileY;

    int numX = xPlayer - xFrustum / 2;
    int numY = yPlayer - yFrustum / 2;

    std::unordered_set<std::pair<int, int>, pair_hash> lineTiles;

    if (world.placementMode == PlacementMode::LINE && gameState.placingState.placing) {
        auto line = bresenham(gameState.placingState.corner.first, gameState.placingState.corner.second, mouseTileX, mouseTileY);
        lineTiles.insert(line.begin(), line.end());
    }

    auto& registry = world.registry;
    auto& renderablePool = registry.storage<Renderable>();

    for (int y = yPlayer - yFrustum / 2; y < (yPlayer + yFrustum / 2) + 1; y++) {

        float screenY = gameState.cameraState.scrHeight - ((y - numY + 1) * settings.yTextSpacing);
        for (int x = xPlayer - xFrustum / 2; x < (xPlayer + xFrustum / 2) + 1; x++) {

            float screenX = (x - numX) * settings.xTextSpacing;
            Tile* a = nullptr;
            Chunk* chunk = nullptr;
            if (world.isRendered()) {
                int chunkX = x / chunkDim;
                int chunkY = y / chunkDim;
                chunk = world.getChunk(chunkX, chunkY);
            }

            wchar_t string;
            glm::vec3 color(1.0f, 1.0f, 1.0f); // default white

            if (gameState.viewState.viewUI) {
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

                        RenderText(shader, string, screenX, screenY, settings.font_size, color);
                        continue;
                    }
                }
            }

            if (!world.isRendered() || !chunk) {
                continue;
            }

            Tile& tile = getTileRef(x, y);

            // Tile contents
            const auto& itemsOnTile = world.objectManager.getObjectsAt(x, y);

            bool entityDrawn = false;

            if (!itemsOnTile.empty()) {
                entt::entity topEntity = itemsOnTile.back();
                if (registry.valid(topEntity) && renderablePool && renderablePool.contains(topEntity)) {
                    const auto& renderable = renderablePool.get(topEntity);
                    color = renderable.color;
                    string = renderable.character;
                    entityDrawn = true;
                }
            }

            if (!entityDrawn) {
                string = tile.character;
                color = tile.color;
            }

            if (tile.anim.type != animType::NONE) applyAnimation(tile, color, string);

            if (gameState.placingState.placing) {
                switch (world.placementMode) {
                case (PlacementMode::SQUARE): {
                    int left = std::min(gameState.placingState.corner.first, mouseTileX);
                    int right = std::max(gameState.placingState.corner.first, mouseTileX);
                    int top = std::min(gameState.placingState.corner.second, mouseTileY);
                    int bottom = std::max(gameState.placingState.corner.second, mouseTileY);

                    if (x >= left && x <= right && y >= top && y <= bottom) {
                        if (y == top || y == bottom || x == left || x == right) {
                            string = L'+';
                            color = glm::vec3(1.0f, 0.0f, 0.0f);
                        }
                    }
                    break;
                }
                case (PlacementMode::SINGLE): {
                    if (x == mouseTileX && y == mouseTileY) {
                        string = L'X';
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    break;
                }
                case (PlacementMode::LINE): {
                    if (lineTiles.find(std::make_pair(x, y)) != lineTiles.end()) {
                        string = L'+';
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    break;

                }
                case (PlacementMode::FILLED_SQUARE): {
                    int left = std::min(gameState.placingState.corner.first, mouseTileX);
                    int right = std::max(gameState.placingState.corner.first, mouseTileX);
                    int top = std::min(gameState.placingState.corner.second, mouseTileY);
                    int bottom = std::max(gameState.placingState.corner.second, mouseTileY);

                    if (x >= left && x <= right && y >= top && y <= bottom) {
                        string = L'+';
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    break;
                }
                }
            }

            if (gameState.viewState.viewHeightMap) {
                string = L'■';
                color =
                    glm::vec3(
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).r / 255.0f,
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).g / 255.0f,
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).b / 255.0f
                    );
            }

            color *= world.getLightMapIndex(x, y);

            if (string != L'\0') {
                RenderText(shader, string, screenX, screenY, settings.font_size, color);
            }
            else {
                RenderText(shader, L' ', screenX, screenY, settings.font_size, glm::vec3(1.0f));
            }
        }
    };
}


void drawMiniMap(Shader& shader, const Settings& settings, World& world) {
    GameState& gameState = Game::getInstance().gameState;

    int xFrustum = gameState.cameraState.xFrustum;
    int yFrustum = gameState.cameraState.yFrustum;

    int xPlayer = gameState.cameraState.xPlayer;
    int yPlayer = gameState.cameraState.yPlayer;

    int numX = xPlayer - xFrustum / 2;
    int numY = yPlayer - yFrustum / 2;
    for (int y = yPlayer - yFrustum / 2; y < yPlayer + yFrustum / 2; y++) {
        for (int x = xPlayer - xFrustum / 2; x < xPlayer + xFrustum / 2; x++) {

            float screenX = (x - numX) * settings.xTextSpacing;
            float screenY = gameState.cameraState.scrHeight - ((y - numY + 1) * settings.yTextSpacing);

            Chunk* chunk = nullptr;

            if (world.isRendered()) {
                int chunkX = static_cast<int>(std::floor((float)x));
                int chunkY = static_cast<int>(std::floor((float)y));
                chunk = world.getChunk(chunkX, chunkY);
            }

            wchar_t charStr;
            glm::vec3 color;

            wchar_t string;

            if (gameState.viewState.viewUI) {
                int uiX = x - (xPlayer - xFrustum / 2);
                int uiY = y - (yPlayer - yFrustum / 2);

                auto& frame = Game::getInstance().getUIManager().getMasterUI();

                if (uiY >= 0 && uiY < frame.size() && uiX >= 0 && uiX < frame[uiY].size()) {
                    wchar_t ch = frame[uiY][uiX];
                    if (ch != L'@') {
                        string = ch;

                        color = (ch == ' ') ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 1.0f, 1.0f);

                        RenderText(shader, string, screenX, screenY, settings.font_size, color);
                        continue;
                    }
                }
            }

            if (!chunk) {
                RenderText(shader, L'≈', screenX, screenY, settings.font_size, glm::vec3(0, 0, 1));
                continue;
            }

            if (!gameState.viewState.viewHeightMap) {
                charStr = chunk->dominantDisplay.character;
                color = chunk->dominantDisplay.color;
            }
            else {
                charStr = L'■';
                color = chunk->dominantDisplay.color == glm::vec3(1.0f) ? glm::vec3(1.0f, 1.0f, 1.0f) :
                    normalizeRGB(glm::vec3(
                        altitudeToColor(chunk->avgHeight, -100.0f, 100.0f).r,
                        altitudeToColor(chunk->avgHeight, -100.0f, 100.0f).g,
                        altitudeToColor(chunk->avgHeight, -100.0f, 100.0f).b
                    ));
            }

            RenderText(shader, charStr, screenX, screenY, settings.font_size, color);
        }
    }
}