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
#include "Inputs.h"

#include "Render/Shader.h"
#include "Utility/Light.h"
#include "Utility/Clock.h"
#include "Jobs/Jobs.h"
#include "Entities/Furnace.h"
#include "Entities/Crop.h"
#include "Entities/Movement.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Audio.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
                    getTileRef(newX, newY).addGroundObject(newX, newY, produce.produce);
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
    for (auto [entity, gun] : view.each()) {

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

void updateNeedsMoving(entt::registry& registry, ObjectManager& objectManager, float deltaTime) {

    // Stockpile item moving
    auto view = registry.view<Position, NeedsMoving>();

    for (auto [entity, pos, moving] : view.each()) {
        if (moving.foundSpot) continue;
        auto& name = registry.get<Name>(entity).name;
        auto spotOpt = mainWorld.findStockpileSpotForItem(name, pos.x, pos.y);

        if (spotOpt) {
            auto& [stockpile, stockpilePos] = *spotOpt;
            stockpile->addItem(entity, stockpilePos.first, stockpilePos.second);

            moving.foundSpot = true;

            Job* job = new HaulToStockpile(entt::null, entt::null, SkillType::None, entity, pos.x, pos.y, stockpilePos.first, stockpilePos.second);

            job->priority = 20;
            JobManager::addJob(job);
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

    // Item setup
    ObjectRegistry::getInstance().loadObjects();
    ObjectRegistry::getInstance().loadStaticObjects();

    Game::getInstance().initUI();

    auto& uiManager = Game::getInstance().getUIManager();
	uiManager.resize(gameState.cameraState.xFrustum, gameState.cameraState.yFrustum);
    uiManager.push(UI::Main);
	
    double lastTime = glfwGetTime();
    int nbFrames = 0;

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
            mainWorld.getLightManager().BFSLight(mainWorld.registry);
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

            MovementManager movement(mainWorld.registry, mainWorld.objectManager);
            movement.updateMovementSystem(dt);

			updateProduceSystem(mainWorld.registry, mainWorld.objectManager, dt);

            CropManager crop(mainWorld.registry);
			crop.updateCropSystem(dt);

            updateHealth(mainWorld.registry, mainWorld.objectManager, dt);
            updateTemperatureSystem(mainWorld.registry, mainWorld.objectManager, dt);
			updateLightSystem(mainWorld.registry, mainWorld.objectManager, dt);
            updateSpawners(mainWorld.registry, mainWorld.objectManager, dt);

            FurnaceManager furnace(mainWorld.registry);
            furnace.updateFurnaces(dt);

            updateNeedsMoving(mainWorld.registry, mainWorld.objectManager, dt);

            mainWorld.squadManager.updateSquadComponent(mainWorld.registry);

            VillagerSystem(dt);
            JobManager::update();

        
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
