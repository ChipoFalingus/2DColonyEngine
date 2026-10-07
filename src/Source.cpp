#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

//#ifdef _DEBUG
//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif

#include <entt/entt.hpp>
#include "Render/Render.h"

#include "Settings.h"
#include "World/Chunk.h"
#include "World/World.h"
#include "Game.h"
#include "Entities/CreatureComponents.h"
#include "Entities/Squad.h"
#include "Utility/itemUtils.h"

#include "Render/Shader.h"
#include "Utility/Light.h"
#include "Utility/Clock.h"
#include "Jobs/Jobs.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Audio.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H


void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void setMode(Mode mode);

std::random_device rd;
int seed;

sf::Font font;
sf::Text text;
std::string textString;

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

// Below methods should be moved somewhere else
void updateMovementSystem(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Position, Movable>();

    for (auto [entity, pos, movable] : view.each()) {

        if (movable.dirX != 0 || movable.dirY != 0) {
            movable.hasTarget = false;
            movable.movementClock += deltaTime;

            if (movable.movementClock >= movable.currentSpeed) {
                movable.movementClock = 0.0f;
                int sepX = 0, sepY = 0;

                forEachInRange(pos.x, pos.y, 3, [&](entt::entity otherEntity, entt::registry& reg, int x, int y) {
                    if (otherEntity == entity) return;
                    if (!reg.try_get<SquadMemberComponent>(otherEntity)) return;
                    auto& otherPos = reg.get<Position>(otherEntity);

                    int dx = pos.x - otherPos.x;
                    int dy = pos.y - otherPos.y;

                    if (dx * dx + dy * dy <= 3) {
                        sepX += dx;
                        sepY += dy;
                    }

                    });

                int pushX = (sepX > 0) - (sepX < 0);
                int pushY = (sepY > 0) - (sepY < 0);

                int stepX = std::clamp(movable.dirX + pushX, -1, 1);
                int stepY = std::clamp(movable.dirY + pushY, -1, 1);

                int newX = pos.x + stepX;
                int newY = pos.y + stepY;

                if (getTileRef(newX, newY).walkable) {
                    objectManager.removeItem(pos.x, pos.y, entity);
                    objectManager.addObject(newX, newY, entity);
                    pos.x = newX;
                    pos.y = newY;
                }

                movable.dirX = 0;
                movable.dirY = 0;
                continue;
            }
        }

        if (!movable.hasTarget) continue;
        movable.movementClock += deltaTime;

        if (movable.movementClock >= movable.currentSpeed) {
            movable.movementClock = 0.0f;

            if (movable.path.empty()) {
                movable.path = findPath(pos.x, pos.y, { movable.targetX, movable.targetY });

                if (movable.path.empty()) {
                    movable.hasTarget = false;
                    continue;
                }
            }

            if (!movable.path.empty()) {
                int newX = movable.path[0].first;
                int newY = movable.path[0].second;

                if (!getTileRef(newX, newY).walkable) {
                    movable.path = findPath(pos.x, pos.y, { movable.targetX, movable.targetY });
                    newX = movable.path[0].first;
                    newY = movable.path[0].second;
                }

                objectManager.removeItem(pos.x, pos.y, entity);
                objectManager.addObject(newX, newY, entity);

                pos.x = newX;
                pos.y = newY;

                movable.path.erase(movable.path.begin());
            }

            if (pos.x == movable.targetX && pos.y == movable.targetY) {
                movable.hasTarget = false;
                movable.path.clear();
            }
        }
    }
}

void updateCropSystem(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Position, Name, Renderable, Crop>();
    for (auto [entity, pos, name, renderable, crop] : view.each()) {
        crop.growthClock += deltaTime;
        if (crop.growthClock >= crop.currentGrowthTime) {

            // hard cap in case the death thing isnt added for a plant
            if (crop.growthStage >= crop.growthStages.size() - 1 || crop.state == CropState::DEAD) {
                crop.growthStage = crop.growthStageMax;
                continue;
            }

            crop.growthClock = 0.0f;
            crop.growthStage++;

            const float variance = getRandomFloat(-0.25f, 0.25f);
            crop.currentGrowthTime = crop.growthTime * (1.0f + variance);

            if (crop.growthStage == crop.mature_stage) {
                crop.state = CropState::MATURED;
                registry.emplace<Matured>(entity);
                crop.currentGrowthTime *= 2;
            }
            else if (crop.growthStage == crop.death_age) {
                registry.erase<Matured>(entity);
                crop.state = CropState::DEAD;
            }
            else {
                crop.state = CropState::SPROUT;
            }

            const auto& currentStage = crop.growthStages[crop.growthStage];
			renderable.character = currentStage.character;
			renderable.color = currentStage.color;
        }
    }
}

constexpr std::array<std::pair<int, int>, 4> dirs = {{
    {0, 1}, {1, 0}, {0, -1}, {-1, 0} 
    }};

void updateProduceSystem(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Position, ProduceSpawner, Matured>();
    for (auto [entity, pos, produce] : view.each()) {

        produce.produceClock += deltaTime;
        if (produce.produceClock >= produce.productionTime) {
            for (auto& dir : dirs) {

                int newX = pos.x + dir.first;
                int newY = pos.y + dir.second;

                if (objectManager.isEmpty(newX, newY) && getTileRef(newX, newY).walkable) {
                    getTileRef(newX, newY).addObject(newX, newY, produce.produce);
                    produce.produceClock = 0.0f;
                    break;
                }
            }
        }
    }
}

void updateHealth(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Health, Position>();
    std::vector<entt::entity> dead;

    for (auto [entity, health, pos] : view.each()) {
        if (health.health <= 0) {
            dead.push_back(entity);
        }
    }

    for (auto entity : dead) {
        if (auto job_component = registry.try_get<JobComponent>(entity)) {
            if (job_component->currentJob) {
                job_component->currentJob->villager = entt::null;
            }
			job_component->clearInterruptedJobs();
        }

        auto& pos = registry.get<Position>(entity);
        getTileRef(pos.x, pos.y).removeObject(pos.x, pos.y, entity);

    }
}

void updateLightSystem(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Position, LightEmitter>();
    for (auto [entity, pos, light] : view.each()) {
        if (!light.addedToLightMap) {
			light.addedToLightMap = true;
            mainWorld.getLightManager().addLight(glm::vec2(pos.x, pos.y), glm::vec3(1.0f), 10.0f, light.light_intensity, -1.0f);
        }
    }
}

void updateTemperatureSystem(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Position, HeatEmitter>();
    for (auto [entity, pos, heat] : view.each()) {
        if (!heat.addedToHeatMap) {
            heat.addedToHeatMap = true;
            Game::getInstance().getHeatManager().addHeatSource(pos.x, pos.y, heat.heat_intensity, 10);
        }
    }
}

void updateSpawners(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Position, Spawner>();
    for (auto [entity, pos, spawner] : view.each()) {

        spawner.clock += deltaTime;
        if (spawner.clock >= spawner.cooldown) {
            spawner.clock = 0.0f;

            for (auto& dir : dirs) {

                int newX = pos.x + dir.first;
                int newY = pos.y + dir.second;

                if (getTileRef(newX, newY).walkable) {
                    auto& registry = mainWorld.registry;
                    entt::entity squadEntity = registry.create();
                    auto& controller = registry.emplace<SquadController>(squadEntity);

                    controller.groupTargetPos = { newX, newY };
                    controller.state = SquadState::IDLE;

                    for (int i = 0; i < 1; i++) {
                        entt::entity member = registry.create();

                        int spawnX = getRandomInt(newX - 10, newX + 10);
                        int spawnY = getRandomInt(newY - 10, newY + 10);

                        if (!getTileRef(spawnX, spawnY).walkable) {
                            i--;
                            continue;
                        }

                        registry.emplace<Position>(member, spawnX, spawnY);
                        registry.emplace<Renderable>(member, L'Z', glm::vec3(0.0f, 1.0f, 0.0f));

                        registry.emplace<Movable>(member);

                        registry.emplace<SquadMemberComponent>(member, squadEntity);

                        registry.emplace<Name>(member, "Zombie");
                        registry.emplace<Hostile>(member);
                        registry.emplace<Health>(member, 40);

                        registry.emplace<Zombie>(member);
                        mainWorld.objectManager.addObject(spawnX, spawnY, member);

                        controller.members.push_back(member);
                    }
                    break;
                }
            }
        }
    }
}

void updateGuns(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {
    auto view = registry.view<Gun>();
    for (auto entity : view) {

        auto& gun = view.get<Gun>(entity);

        if (gun.reloading) {
            gun.reload_clock += deltaTime;

            if (gun.reload_clock >= gun.reload_time) {
                int neededAmmo = gun.ammo_capacity - gun.current_ammo;
                gun.current_ammo = neededAmmo;
                gun.reloading = false;
            }
        }
    }
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GameState& gameState = Game::getInstance().gameState;

    int screenWidth = 1920;
    int screenHeight = 1080;

    gameState.cameraState.scrWidth = screenWidth;
    gameState.cameraState.scrWidth = screenHeight;

    //scrWidth = mode->width;
    //scrHeight = mode->height;

    gameState.cameraState.xFrustum = screenWidth / Game::getInstance().getSettingsManager().get().xTextSpacing;
    gameState.cameraState.yFrustum = screenHeight / Game::getInstance().getSettingsManager().get().xTextSpacing;


    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "ASCII Game", NULL, NULL);
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

    glViewport(0, 0, screenWidth, screenHeight);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Original is 48 * 0.55f
    generateFontAtlas("Fonts/cour.ttf", /*(float)Game::getInstance().getSettingsManager().get().font_size*/ 48 * 0.55);

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

    Shader shader("assets/shader.vert", "assets/shader.frag");

    std::random_device rd;
    seed = rd();
    rng.seed(seed);
    srand(seed);

    // Item setup
    ObjectRegistry::getInstance().loadObjects();
    ObjectRegistry::getInstance().loadStaticObjects();

    Game::getInstance().initUI();

    auto& uiManager = Game::getInstance().getUIManager();
	uiManager.resize(gameState.cameraState.xFrustum, gameState.cameraState.yFrustum);
    uiManager.push(UI::Main);
	
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    std::string fps;

    sf::Music music;

    if (!music.openFromFile("assets/music.mp3")) {
        std::cout << "Music failed to load!" << std::endl;
    }

    //music.play();

    framebuffer_size_callback(window, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSwapInterval(0);

    float lightClock = 0.0f;
	float heatClock = 0.0f;
	float waitingUpdateTimer = 0.0f;

    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        float dt = (currentTime - lastTime);
        lastTime = currentTime;

        Clock::update(dt);

        Game::getInstance().getSettingsUI().update();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ClearBatch();

        // Draw text
        if (gameState.viewState.viewMiniMap) {
            drawMiniMap(shader, Game::getInstance().getSettingsManager().get(), mainWorld);

            int chunkX = static_cast<int>(std::floor((float)gameState.inputState.mouseTileX));
            int chunkY = static_cast<int>(std::floor((float)gameState.inputState.mouseTileY));
            Chunk* chunk = mainWorld.getChunk(chunkX, chunkY);
            Game::getInstance().getMiniMapUI().update(chunk);
        }
        else {
            drawMap(shader, Game::getInstance().getSettingsManager().get(), mainWorld);
        }

		FlushBatch(shader);

        // Inputs
        processInput(window);

        mainWorld.getLightManager().update(dt);

        // Update lighting
        // change this to only update when dirty
        lightClock += dt;
        if (lightClock > .1f) {
			lightClock = 0.0f;
            mainWorld.getLightManager().BFSLight();
        }

        // Update temperature
        // same here
		heatClock += dt;
        if (heatClock > .5f) {
            heatClock = 0.0f;
            std::vector<float> heatMap = Game::getInstance().getHeatManager().calculateHeatMap(calculateMapSize());
            mainWorld.setTemperatureMap(heatMap);
        }

        glfwGetCursorPos(window, &gameState.inputState.mouseX, &gameState.inputState.mouseY);

        int xTextSpacing = Game::getInstance().getSettingsManager().get().xTextSpacing;
        int yTextSpacing = Game::getInstance().getSettingsManager().get().yTextSpacing;

        gameState.inputState.mouseTileX = (int)(gameState.inputState.mouseX / xTextSpacing) + gameState.cameraState.xPlayer - (gameState.cameraState.scrWidth / (2 * xTextSpacing));
        gameState.inputState.mouseTileY = (int)(gameState.inputState.mouseY / yTextSpacing) + gameState.cameraState.yPlayer - (gameState.cameraState.scrHeight / (2 * yTextSpacing));

        if (gameState.viewState.viewUI) {
            uiManager.update(gameState.inputState.mouseX / xTextSpacing, gameState.inputState.mouseY / yTextSpacing, gameState.inputState.clicked);
            uiManager.draw();
        }

        if (mainWorld.isCurrentlyRendering()) {
            Game::getInstance().getLoadingUI().update();
        }

        if (mainWorld.isRendered()) {
			mainWorld.dayCycle.update();
			updateMovementSystem(mainWorld.registry, mainWorld.objectManager, dt);
			updateProduceSystem(mainWorld.registry, mainWorld.objectManager, dt);
			updateCropSystem(mainWorld.registry, mainWorld.objectManager, dt);
            updateHealth(mainWorld.registry, mainWorld.objectManager, dt);
            updateTemperatureSystem(mainWorld.registry, mainWorld.objectManager, dt);
			updateLightSystem(mainWorld.registry, mainWorld.objectManager, dt);
            updateSpawners(mainWorld.registry, mainWorld.objectManager, dt);

            mainWorld.squadManager.updateSquadComponent(mainWorld.registry);

            VillagerSystem(dt);
            JobManager::update();

            // Stockpile item moving
            auto view = mainWorld.registry.view<Position, NeedsMoving>();

            for (auto [entity, pos, moving] : view.each()) {
                if (moving.foundSpot) continue;

                auto& name = mainWorld.registry.get<Name>(entity).name;
                auto spotOpt = mainWorld.findStockpileSpotForItem(name, pos.x, pos.y);

                if (spotOpt) {
                    auto& [stockpile, stockpilePos] = *spotOpt;
                    stockpile->addItem(entity, stockpilePos.first, stockpilePos.second);

                    moving.foundSpot = true;

                    Job* job = new HaulToStockpile(entt::null, entt::null, SkillType::None, entity, pos.x, pos.y, stockpilePos.first, stockpilePos.second);

                    job->priority = 25;
                    JobManager::addJob(job);
                }
            }
        
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
       
    GameState& gameState = Game::getInstance().gameState;

    gameState.cameraState.scrWidth = width;
    gameState.cameraState.scrHeight = height;
    gameState.cameraState.xFrustum = width / Game::getInstance().getSettingsManager().get().xTextSpacing;
    gameState.cameraState.yFrustum = height / Game::getInstance().getSettingsManager().get().yTextSpacing;
    std::cout << "Tile dimesions resized to " << gameState.cameraState.xFrustum << "x" << gameState.cameraState.yFrustum << std::endl;

	auto& uiManager = Game::getInstance().getUIManager();
	uiManager.resize(gameState.cameraState.xFrustum, gameState.cameraState.yFrustum);
    for (auto& i : uiManager.getAllFrames()) {
		auto frame = i.second.get();
        frame->resize(gameState.cameraState.xFrustum, gameState.cameraState.yFrustum);
    }
}
