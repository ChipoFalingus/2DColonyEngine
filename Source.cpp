#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <thread>
#include <future>

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

#include "Pig.h"
#include "Zombie.h"

#include "Shader.h"
#include "Light.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Audio.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H




void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

std::random_device rd;
int seed = rd();

sf::Clock loadTimer;

const int MAP_WIDTH = 10; //420
const int MAP_HEIGHT = 10; //275


float water = 0.0f;

int hue = 0;


static std::vector<float> batchVertices; // flattened: x,y,u,v,r,g,b per vertex
static const size_t BATCH_VERTEX_SIZE = 7; // floats per vertex
static const size_t QUAD_VERTS = 6; // two triangles per quad

std::vector<unsigned char> wallData;

std::vector<std::vector<wchar_t>> menuArray;
std::vector<std::vector<wchar_t>> worldMenuArray;
float mapSize = 1.0f;



sf::Font font;
sf::Text text;
std::string textString;


GLuint VAO, VBO;
GLuint EBO;
GLuint batchVAO, batchVBO;


std::vector<Creature*> Creature::allCreatures;


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


std::vector<Light> LightManager::lights;

std::map<wchar_t, Character> Characters;

GLuint fontTexture;

std::vector<float> vertices;


std::vector<std::pair<int, int>> activeWater;

void generateFontAtlas(const std::string& fontPath, int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) { std::cerr << "Could not init FreeType\n"; return; }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) { std::cerr << "Failed to load font\n"; return; }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const int atlasWidth = 8192;
    const int atlasHeight = 8192;
    unsigned char* atlasData = new unsigned char[atlasWidth * atlasHeight];
    memset(atlasData, 0, atlasWidth * atlasHeight);

    int x = 0, y = 0, rowHeight = 0;

    for (wchar_t c = 0; c < 65535; c++) { // printable ASCII
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        FT_Bitmap& bmp = face->glyph->bitmap;

        // check if we need to move to next row
        if (x + bmp.width >= atlasWidth) { x = 0; y += rowHeight; rowHeight = 0; }
        if (y + bmp.rows >= atlasHeight) { std::cerr << "Atlas too small!\n"; break; }

        // copy glyph bitmap into atlas
        for (int row = 0; row < bmp.rows; row++)
            for (int col = 0; col < bmp.width; col++)
                atlasData[(y + row) * atlasWidth + (x + col)] = bmp.buffer[row * bmp.width + col];

        // store character info
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
            //tile.items[0] = std::make_unique<Item>(DISPLAY);
            tile.items[0]->displayChar = ch;
        }
        offsetY++;
    }
}

static void ClearBatch() {
    batchVertices.clear();
}

static void FlushBatch(Shader& shader) {
    if (batchVertices.empty()) return;

    // set shader and per-frame uniforms (projection + sampler + isUI defaults to false)
    float zoom = 1.0f;
    glm::mat4 projection = glm::ortho(0.0f, (float)scrWidth * zoom, 0.0f, (float)scrHeight * zoom);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setInt("text", 0);      // ensure sampler unit 0
    shader.setBool("isUI", false); // RenderText still sets per-glyph isUI during batching but ensure default

    // set lights
    shader.setInt("numLights", LightManager::lights.size());
    for (int i = 0; i < LightManager::lights.size(); i++) {
        std::string base = "lights[" + std::to_string(i) + "]";
        shader.setVec2(base + ".position", LightManager::lights[i].position);
        shader.setVec3(base + ".color", LightManager::lights[i].color);
        shader.setFloat(base + ".radius", LightManager::lights[i].radius);
        shader.setFloat(base + ".additionalIntensity", LightManager::lights[i].additionalIntensity);
    }


    int width = xFrustum;
    int height = yFrustum;

    std::vector<unsigned char> wallData(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            wallData[y * width + x] = getTileRef(x, y).walkable ? 1 : 0;
        }
    }

    GLuint wallTex;
    glGenTextures(1, &wallTex);
    glBindTexture(GL_TEXTURE_2D, wallTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0,
    GL_RED, GL_UNSIGNED_BYTE, wallData.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    shader.setInt("wallMap", 0);
    
	shader.setInt("mapWidth", width);
	shader.setInt("mapHeight", height);

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

//std::vector<std::vector<wchar_t>> txtTo2DArray(const std::string& filePath) {
//    std::wifstream file(filePath);
//    file.imbue(std::locale("en_US.UTF-8"));
//
//    if (!file.is_open()) {
//        std::cerr << "Failed to open file: " << filePath << std::endl;
//        return {};
//    }
//
//    std::vector<std::vector<wchar_t>> array;
//    std::wstring line;
//    while (std::getline(file, line)) {
//        if (!line.empty() && line[0] == 0xFEFF) {
//            line.erase(0, 1);
//        }
//        std::vector<wchar_t> row(line.begin(), line.end());
//        array.push_back(row);
//    }
//    return array;
//}

void renderMap() {

    int mapDim = calculateMapSize();

    for (int y = -mapDim; y < mapDim; y++) {
        for (int x = -mapDim; x < mapDim; x++) {
            int chunkX = static_cast<int>(std::floor((float)x / chunkDim));
            int chunkY = static_cast<int>(std::floor((float)y / chunkDim));

            Chunk& chunk = loadOrGenerateChunk(chunkX, chunkY);
        }
    }

    for (int y = -mapDim; y < mapDim; y++) {
        for (int x = -mapDim; x < mapDim; x++) {
            Tile& tile = getTileRef(x, y);
            tile.getTile(x, y);
        }
    }
    

  //  for (int y = -mapDim; y < mapDim; y++) {
  //      for (int x = -mapDim; x < mapDim; x++) {
  //          Tile& tile = getTileRef(x, y);
  //          if (110.0f < tile.altitude && getRandomInt(0, 0) == 0) {
  //             // makeRiver(x, y);
  //              //tile.water = 1.0f;
		//		
  //              auto basin = makeRiver(x, y);
  //              getTileRef(basin.first, basin.second).water = 10000.0f;
  //              

  //              addBasin(basin.first, basin.second);


  //          }
  //      }
  //  }

  //  for (int k = 0; k < 0; k++) {
  //      for (int i = -mapDim; i < mapDim; i++) {
  //          for (int j = -mapDim; j < mapDim; j++) {
  //              Tile& tile = getTileRef(i, j);
  //              tile.simulateWaterTile();
  //          }
  //      }
		//std::cout << k << " water simulation step complete." << std::endl;
  //  }

    

    
	updateMiniMap();
}

void RenderText(Shader& shader, const std::wstring& text, float x, float y, float scale, glm::vec3 color, bool ui) {
    // We still prepare projection and shader-state per call, but do not upload/draw here.
    float zoom = 1.0f;
    glm::mat4 projection = glm::ortho(0.0f, (float)scrWidth * zoom, 0.0f, (float)scrHeight * zoom);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setInt("text", 0);
    shader.setBool("isUI", ui);

    // NOTE: we DO NOT set light uniforms here (FlushBatch will), but setting them here is harmless.
    // Build quads into batchVertices
    for (wchar_t c : text) {
        if (Characters.find(c) == Characters.end()) continue;
        Character ch = Characters[c];

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
    }
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

            int chunkX = static_cast<int>(std::floor((float)x));
            int chunkY = static_cast<int>(std::floor((float)y));

            int localX = (x % chunkDim + chunkDim) % chunkDim;
            int localY = (y % chunkDim + chunkDim) % chunkDim;

            float screenX = (x - numX) * xTextSpacing;
            float screenY = scrHeight - ((y - numY + 1) * yTextSpacing);

            Chunk& chunk = loadOrGenerateChunk(chunkX, chunkY);

            std::wstring charStr;
            glm::vec3 color;

            if (!viewHeightMap) {
                // Convert vector<wchar_t> to wstring(use first character if available)
                charStr = chunk.dominantDisplay.character.empty() ? L" " : std::wstring(1, chunk.dominantDisplay.character[0]);
                // Convert vector<sf::Color> to glm::vec3 (use first color if available)
                color = chunk.dominantDisplay.color.empty() ? glm::vec3(1.0f, 1.0f, 1.0f) :
                    glm::vec3(
                        chunk.dominantDisplay.color[0].r / 255.0f,
                        chunk.dominantDisplay.color[0].g / 255.0f,
                        chunk.dominantDisplay.color[0].b / 255.0f
                    );
            }
            else {
                // Convert vector<wchar_t> to wstring(use first character if available)
                charStr = L'■';
                // Convert vector<sf::Color> to glm::vec3 (use first color if available)
                color = chunk.dominantDisplay.color.empty() ? glm::vec3(1.0f, 1.0f, 1.0f) :
                    glm::vec3(
                        altitudeToColor(chunk.avgHeight, -100.0f, 100.0f).r / 255.0f,
                        altitudeToColor(chunk.avgHeight, -100.0f, 100.0f).g / 255.0f,
                        altitudeToColor(chunk.avgHeight, -100.0f, 100.0f).b / 255.0f
                    );
            }
            

            RenderText(shader, charStr, screenX, screenY, fontSize, color, false);
        }
    }
}

void drawMap(Shader& shader)
{
    int numX = xPlayer - xFrustum / 2;
    int numY = yPlayer - yFrustum / 2;
	

    // Iterate over camera frustum
    for (int y = yPlayer - yFrustum / 2; y < yPlayer + yFrustum / 2; y++) {
        std::wstring line;
        float screenY = scrHeight - ((y - numY + 1) * yTextSpacing);
        for (int x = xPlayer - xFrustum / 2; x < xPlayer + xFrustum / 2; x++) {

            int chunkX = static_cast<int>(std::floor((float)x / chunkDim));
            int chunkY = static_cast<int>(std::floor((float)y / chunkDim));
			
            Chunk& chunk = loadOrGenerateChunk(chunkX, chunkY);

            /*std::future<Chunk&> future = std::async(std::launch::async, loadOrGenerateChunk, chunkX, chunkY);
            Chunk& chunk = future.get();*/

            int localX = (x % chunkDim + chunkDim) % chunkDim;
            int localY = (y % chunkDim + chunkDim) % chunkDim;

            Tile& tile = chunk.tiles[localX][localY];

            // Calculate screen position in pixels
            float screenX = (x - numX) * xTextSpacing;
            

            tile.update();
            std::wstring string;
            glm::vec3 color(1.0f, 1.0f, 1.0f); // default white

            int uiX = x - (xPlayer - xFrustum / 2);
            int uiY = y - (yPlayer - yFrustum / 2);

            
            if (uiY >= 0 && uiY < menuArray.size() && uiX >= 0 && uiX < menuArray[uiY].size()) {
                wchar_t ch = menuArray[uiY][uiX];
                if (ch != L'#') {
                    string = std::wstring(1, ch);
                    if (ch == L' ')
                        color = glm::vec3(0.0f, 0.0f, 0.0f);
                    else
                        color = glm::vec3(1.0f, 1.0f, 1.0f);
                    RenderText(shader, string, screenX, screenY, fontSize, color, true);
                    //line += string;
                    continue;
                }
            }

			bool foundCreature = false;
            for (Creature* v : Creature::allCreatures) {
                
                if (v->xPos == x && v->yPos == y) {
					string = v->displayChar;
                    color = glm::vec3(v->displayColor.r / 255.0f, v->displayColor.g / 255.0f, v->displayColor.b / 255.0f);
                    v->xPixels = screenX;
                    v->yPixels = screenY;
                    foundCreature = true;
                    break;
                }
                
                if (v->xPos < xPlayer - xFrustum / 2 || v->yPos > yPlayer + yFrustum / 2
                    || v->xPos > xPlayer + xFrustum / 2 || v->yPos < yPlayer - yFrustum / 2) {
                    v->xPixels = -99999999;
                    v->yPixels = -99999999;
                }
            }

            // Tile contents
            if (!foundCreature) {
                if (tile.items.size() != 0) {
                    if (tile.items[0]->name == "") {
                        string = tile.character;
                        color = glm::vec3(tile.color.r / 255.0f, tile.color.g / 255.0f, tile.color.b / 255.0f);
                    }
                    else {
                        string = tile.items[0]->displayChar;
                        color = glm::vec3(tile.items[0]->displayColor.r / 255.0f,
                            tile.items[0]->displayColor.g / 255.0f,
                            tile.items[0]->displayColor.b / 255.0f);
                    }
                }
                else {
                    string = tile.character;
                    color = glm::vec3(tile.color.r / 255.0f, tile.color.g / 255.0f, tile.color.b / 255.0f);
                }

                if (placing) {
                    int left = std::min(corner.first, mouseTileX);
                    int right = std::max(corner.first, mouseTileX);
                    int top = std::min(corner.second, mouseTileY);
                    int bottom = std::max(corner.second, mouseTileY);

                    if ((y == top && x >= left && x <= right) ||
                        (y == bottom && x >= left && x <= right) ||
                        (x == left && y >= top && y <= bottom) ||
                        (x == right && y >= top && y <= bottom)) {
                        string = L"+";
                        color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                }
            }

            if (tile.water > 0.0f) {

                // Water character
                string = L'≈';

                // Clamp water level (0.0 → 1.0)
                float w = glm::clamp(tile.water, 0.0f, 1.0f);

                // Water color fades with small amounts:
                float r = (1.0f - w) * 1.0f;   // fades toward blue
                float g = (1.0f - w) * 1.0f;
                float b = 1.0f;

                color = glm::vec3(r, g, b);
            }

            /*if (tile.water > 0.01f) {
                string = L'▼';
                color = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            if (tile.water > 0.4f) {
                string = L'≈';
                color = glm::vec3(0.0f, 0.0f, 1.0f);
            }*/


            if (viewHeightMap) {
                string = L'■';
                color =
                    glm::vec3(
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).r / 255.0f,
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).g / 255.0f,
                        altitudeToColor(tile.altitude, -100.0f, 100.0f).b / 255.0f
                    );
            }



            

            


            if (!string.empty()) {
                RenderText(shader, string, screenX, screenY, fontSize, color, false);
            }
        }
        //RenderText(shader, line, 0.0f, screenY, fontSize, glm::vec3(1.0f), false);
    }
}


void storeCharacterTexture(FT_Face face, int c) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // store character
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<GLuint>(face->glyph->advance.x)
    };
    Characters.insert(std::pair<wchar_t, Character>(c, character));
}

int main() {

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    //scrWidth = mode->width;
    //scrHeight = mode->height;

	scrWidth = 1920;
	scrHeight = 1080;

	xFrustum = scrWidth / xTextSpacing;
    yFrustum = scrHeight / yTextSpacing;

    

    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "Me ASCII Game", NULL, NULL);
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


    generateFontAtlas("cour.ttf", 48);




    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // We will allocate dynamically in FlushText, but reserve an initial size
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    // pos (location = 0): vec2
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, BATCH_VERTEX_SIZE * sizeof(float), (void*)(0));

    // uv (location = 1): vec2
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, BATCH_VERTEX_SIZE * sizeof(float), (void*)(2 * sizeof(float)));

    // color (location = 2): vec3
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, BATCH_VERTEX_SIZE * sizeof(float), (void*)(4 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader shader("shader.vert", "shader.frag");

    LightManager::addLight(glm::vec2(scrWidth / 2, scrHeight / 2), glm::vec3(1.0f), 30000.0f, 1.0f, -1.0f);

    /*for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            glm::vec3 color = glm::vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            LightManager::addLight(glm::vec2(400 + i * 200, 300 + j * 150), color, 100.0f, 10.0f, -1.0f);
        }
    }*/

    std::random_device rd;
    seed = rd();
    rng.seed(seed);
    std::cout << "Seed: " << seed << std::endl;
    srand(seed);


    // Item setup
    loadItems();
    loadTools();

	loadHarvestRules();

    // World setup
    initializePermutation();
    createVoronoiMap();
    createMapIslands();
    renderMap();
    setSeaLevel(water);
    calculateMapSize();
    

    //   for (int i = 0; i < 0; i++) {
    //       Creature::allCreatures.push_back(new Pig(getRandomInt(-5,5), getRandomInt(-5, 5)));
    //   }


       for (int i = 0; i < 0; i++) {
		   int x = getRandomInt(-100, 100);
		   int y = getRandomInt(-100, 100);
           if (getTileRef(x, y).walkable) {
               Creature::allCreatures.push_back(new Zombie(x, y));
           }
       }

       int range = 1;


       Tool* axe = ToolRegistry::getInstance().get("Axe");
       axe->material = ToolMaterial::WOOD;
    for (int i = 0; i < 10; i++) {
        Villager* v = new Villager(getRandomInt(-range, range), getRandomInt(-range, range));
		v->jobType = JobType::Lumberjack;
        v->toolInHand = axe;
		Villager::allVillagers.push_back(v);
		Creature::allCreatures.push_back(v);
    }

    for (int i = 0; i < 10; i++) {
        Villager* v = new Villager(getRandomInt(-range, range), getRandomInt(-range, range));
        v->jobType = JobType::Miner;
        v->itemInHand = &PEPPER_GUN;
        Villager::allVillagers.push_back(v);
        Creature::allCreatures.push_back(v);
    }

    for (int i = 0; i < 10; i++) {
        Villager* v = new Villager(getRandomInt(-range, range), getRandomInt(-range, range));
        v->jobType = JobType::Farmer;
        v->itemInHand = &PEPPER_GUN;
        Villager::allVillagers.push_back(v);
        Creature::allCreatures.push_back(v);
    }

    for (int i = 0; i < 10; i++) {
        Villager* v = new Villager(getRandomInt(-range, range), getRandomInt(-range, range));
        v->jobType = JobType::Builder;
        v->itemInHand = &PEPPER_GUN;
        Villager::allVillagers.push_back(v);
        Creature::allCreatures.push_back(v);
    }

    for (int i = 0; i < 1; i++) {
        Villager* v = new Villager(getRandomInt(-range, range), getRandomInt(-range, range));
        v->jobType = JobType::Carpenter;
        //v->itemInHand = &PEPPER_GUN;
        Villager::allVillagers.push_back(v);
        Creature::allCreatures.push_back(v);
    }

    JobManager::addJob(new Craft(nullptr, JobType::Carpenter, &getWoodenPlankRecipe()));

    auto function = [](Creature* c) {
        return dynamic_cast<Zombie*>(c) != nullptr;
    };


    Start.onClick = []() {
        std::cout << "Clicked Start!" << std::endl;
    };

    Options.onClick = []() {
        std::cout << "Clicked Options!" << std::endl;
        };

    Exit.onClick = [window]() {
        glfwSetWindowShouldClose(window, true);
    };

    BuildButton.onClick = []() {
        if (buildMode) {
            buildMode = false;
            harvestMode = false;
            plantMode = false;
			placing = false;
			std::cout << "Build mode off" << std::endl;
        }
        else {
            buildMode = true;
            std::cout << "Build mode on" << std::endl;
        }
    };

    ClearButton.onClick = []() {
        if (harvestMode) {
            buildMode = false;
            harvestMode = false;
            plantMode = false;
            placing = false;
            std::cout << "Harvest mode off" << std::endl;
        }
        else {
            harvestMode = true;
            std::cout << "Harvest mode on" << std::endl;
        }
    };

    PlantButton.onClick = []() {
        if (plantMode) {
            buildMode = false;
            harvestMode = false;
            plantMode = false;
            placing = false;
            std::cout << "Plant mode off" << std::endl;
        }
        else {
            plantMode = true;
            std::cout << "Plant mode on" << std::endl;
        }
	};

    BuildStockpile.onClick = []() {
        if (stockpileMode) {
            stockpileMode = false;
            placing = false;
            std::cout << "Stockpile mode off" << std::endl;
        }
        else {
            stockpileMode = true;
            std::cout << "Stockpile mode on" << std::endl;
        }
    };



    
    ui.init("Info.txt");
    ui.resizeUI(xFrustum, yFrustum);
    /*ui.UIButtons.push_back(Start);
    ui.UIButtons.push_back(Options);
    ui.UIButtons.push_back(Exit);*/
    ui.UIButtons.push_back(BuildButton);
    ui.UIButtons.push_back(ClearButton);
	ui.UIButtons.push_back(PlantButton);
	ui.UIButtons.push_back(BuildStockpile);
    menuArray = ui.UI;

    

    sf::Clock lightClock;
	sf::Clock waterClock;

    sf::Music music;

    if (!music.openFromFile("music.mp3")) {
        std::cout << "Music failed to load!" << std::endl;
    }

    bool firstRender = true;
    //music.play();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::vector<std::pair<int, int>> nextActive = getWater();

    while (!glfwWindowShouldClose(window)) {


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ClearBatch();

        // Draw text
        if (viewMiniMap) {
            drawMiniMap(shader);
        }
        else {
            if (firstRender) {
                std::cout << "Rendered in " << loadTimer.getElapsedTime().asMilliseconds() << " milliseconds." << std::endl;
                firstRender = false;
            }
            drawMap(shader);
        }

        menuArray = ui.UI;
        ui.drawUI();

		FlushBatch(shader);

        // Inputs
        processInput(window);

        // Update creatures
        float deltaTime = lightClock.restart().asSeconds();
        LightManager::update(deltaTime);

        for (auto it = Creature::allCreatures.begin(); it != Creature::allCreatures.end(); ) {
            Creature* v = *it;
            if (v->health < 1) {
                it = Creature::allCreatures.erase(it);
                //getTileRef(v->xPos, v->yPos).addItem(std::make_unique<Item>(blood));
            }
            else {
                v->doWork();
                it++;
            }
        }
        

        glfwGetCursorPos(window, &mouseX, &mouseY);

        mouseTileX = (int)(mouseX / xTextSpacing) + xPlayer - (scrWidth / (2 * xTextSpacing));
        mouseTileY = (int)(mouseY / yTextSpacing) + yPlayer - (scrHeight / (2 * yTextSpacing));
        Tile& tile = getTileRef(mouseTileX, mouseTileY);
        
        

        for (auto& pos : activeWater) {
            Tile& tile = getTileRef(pos.first, pos.second);
            tile.simulateWaterTile();

            if (tile.water > 0.0f && tile.altitude > 0.0f)
                nextActive.push_back(pos);

            for (auto& n : getNeighbors(pos.first, pos.second)) {
                Tile& neighbor = getTileRef(n.first, n.second);
                if (neighbor.water > 0.0f && tile.altitude > 0.0f)
                    nextActive.push_back(n);
            }
        }

        std::sort(nextActive.begin(), nextActive.end());
        nextActive.erase(std::unique(nextActive.begin(), nextActive.end()), nextActive.end());

        activeWater = std::move(nextActive);

		//std::cout << activeWater.size() << " active water tiles." << std::endl;

        if (JobManager::JobList.size() > 0) {
            for (int i = 0; i < JobManager::JobList.size(); i++) {
                JobManager::findBestColonistForJob(*JobManager::JobList[i]);
            }
        }

		// Stockpile item moving
        for (auto it = itemsToMove.begin(); it != itemsToMove.end(); ) {
            bool placed = false;

            for (auto& i : stockpileTiles) {
                Tile& tile = getTileRef(i.location.first, i.location.second);

                if (tile.items.size() == 1 && !i.claimed) {
                    Item* item = it->first;
					Job* job = new MoveItem(nullptr, JobType::None, item, it->second.first, it->second.second, i.location.first, i.location.second);
					job->priority = 5;
					JobManager::addJob(job);
                    i.claimed = true;

                    it = itemsToMove.erase(it);
                    placed = true;
                    break;
                }
            }

            if (!placed)
                ++it;
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
	std::cout << "Window resized to " << width << "x" << height << std::endl;
	scrWidth = width;
	scrHeight = height;
    xFrustum = scrWidth / xTextSpacing;
    yFrustum = scrHeight / yTextSpacing;

    ui.resizeUI(xFrustum, yFrustum);
}