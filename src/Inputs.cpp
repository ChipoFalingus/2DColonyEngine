#include "Inputs.h"
#include "Jobs/Job.h"
#include "World/Tile.h"
#include "World/World.h"
#include "Game.h"
#include "UI/UI.h"
#include "Entities/ItemComponents.h"
#include "Entities/CreatureComponents.h"
#include "Entities/Squad.h"
#include "Entities/Gun.h"
#include "Entities/Inventory.h"
#include "Entities/Movement.h"
#include "Jobs/Jobs.h"

double lastTime = glfwGetTime();
int nbFrames = 0;
std::string fps;

float moveClock = 0.0f;


void handleMode();
void handleClickedItem(int x, int y);
void build(int left, int right, int top, int bottom);
void harvest(int left, int right, int top, int bottom);
void plant(int left, int right, int top, int bottom);
void stockpile(int left, int right, int top, int bottom);


class Inputs {
private:
    std::unordered_set<int> held_keys;

public:
    bool is_key_pressed(GLFWwindow* window, int key, bool can_hold) {
        bool is_down = (glfwGetKey(window, key) == GLFW_PRESS);

        if (!is_down) {
            held_keys.erase(key);
            return false;
        }

        if (can_hold) {
            return true;
        }

        if (held_keys.find(key) != held_keys.end()) {
            return false;
        }

        held_keys.insert(key);
        return true;
    }
};

Inputs inputs;

// have you ever gotten post-code clarity and realized this is bad
void processInput(GLFWwindow* window) {

    moveClock += Clock::deltaTime;

    GameState& gameState = Game::getInstance().gameState;

    int mX = gameState.inputState.mouseTileX;
    int mY = gameState.inputState.mouseTileY;

    if (moveClock > 1.0f / (float)Game::getInstance().getSettingsManager().get().camera_speed) {
        moveClock = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            gameState.cameraState.yPlayer--;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            gameState.cameraState.yPlayer++;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            gameState.cameraState.xPlayer--;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            gameState.cameraState.xPlayer++;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !gameState.inputState.clicked) {

        if (gameState.placingState.currentMode != Mode::NONE) {
            if (!gameState.placingState.placing) {
                gameState.placingState.corner = { mX, mY };
                gameState.placingState.placing = true;
            }
            else {
                handleMode();

                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
                {
                    gameState.placingState.placing = false;
                    gameState.placingState.currentMode = Mode::NONE;
                }
            }
        }
        else {

            if (mainWorld.isRendered()) {
                gameState.placingState.placing = false;
                handleClickedItem(mX, mY);

				//getTileRef(gameState.inputState.mouseTileX, gameState.inputState.mouseTileY).addObject(gameState.inputState.mouseTileX, gameState.inputState.mouseTileY, "Guard Post");
            }
        }

        gameState.inputState.clicked = true;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        gameState.inputState.clicked = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        gameState.placingState.placing = false;
        setMode(Mode::NONE);

        if (Game::getInstance().v_villager != entt::null) {
            entt::entity v = Game::getInstance().v_villager;

            auto& jobComponent = mainWorld.registry.get<JobComponent>(v);
            for (auto item : mainWorld.objectManager.getObjectsAt(mX, mY)) {

                if (!mainWorld.registry.all_of<Gun>(item)) continue;

                auto* claimable = mainWorld.registry.try_get<Claimable>(item);
                if (claimable && claimable->claimed) continue;

                Job* job = new EquipItem(v, entt::null, SkillType::None, item);
                job->priority = 100.0f;
                if (jobComponent.willAccept(job) && !mainWorld.registry.get<Inventory>(v).is_full()) {
                    if (claimable) claimable->claimed = true;
                    jobComponent.proposeJob(std::move(job));
                    break;
                }
                else {
                    delete job;
                }
            }
        }
    }

    if (inputs.is_key_pressed(window, GLFW_KEY_U, false)) {
        gameState.placingState.placing = false;
        setMode(Mode::NONE);

        for (auto& i : mainWorld.objectManager.getObjectsAt(mX, mY)) {
            if (mainWorld.registry.all_of<Desconstructable>(i)) {
                std::printf("Added deconstruction job");

                // job here
                Job* job = new Deconstruct(entt::null, entt::null, SkillType::None, i);
                job->priority = 25;

                JobManager::addJob(std::move(job));
            }
        }
    }

    if (inputs.is_key_pressed(window, GLFW_KEY_M, false)) {
        if (!gameState.viewState.viewMiniMap) {
            gameState.viewState.viewMiniMap = true;
        }
        else if (gameState.viewState.viewMiniMap) {
            gameState.viewState.viewMiniMap = false;
        }

        auto& UIManager = Game::getInstance().getUIManager();
		UIManager.swapFrame(UI::Minimap, UI::InGame);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        gameState.viewState.viewHeightMap = !gameState.viewState.viewHeightMap;
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        gameState.cameraState.xPlayer = 0;
        gameState.cameraState.yPlayer = 0;
    }
    if (inputs.is_key_pressed(window, GLFW_KEY_F2, false)) {
        gameState.viewState.viewUI = !gameState.viewState.viewUI;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        mainWorld.updateMiniMap();
    }

    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
        size_t totalEnTTEntities = mainWorld.registry.storage<entt::entity>().size();

        size_t activeJobs = mainWorld.registry.view<JobComponent>().size();
        size_t physicalObjects = mainWorld.registry.view<Position>().size();

		std::cout << "=== Debug Info ===" << std::endl;
        std::cout << "Total Allocated EnTT IDs: " << totalEnTTEntities << std::endl;
        std::cout << "Entities with Jobs:       " << activeJobs << std::endl;
        std::cout << "Entities with Positions:  " << physicalObjects << std::endl;
    }

    if (inputs.is_key_pressed(window, GLFW_KEY_R, false)) {
		gameState.debugState.roomView = !gameState.debugState.roomView;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        gameState.debugState.objectLocationView = !gameState.debugState.objectLocationView;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        gameState.debugState.jobDanger = !gameState.debugState.jobDanger;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        gameState.debugState.showLightLevel = !gameState.debugState.showLightLevel;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        gameState.debugState.showTemp = !gameState.debugState.showTemp;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        auto& registry = mainWorld.registry;
        entt::entity squadEntity = registry.create();
        auto& controller = registry.emplace<SquadController>(squadEntity);

        controller.groupTargetPos = { mX, mY };
        controller.state = SquadState::IDLE;

        int attemptCount = 0;
        int maxAttempts = 100;
        for (int i = 0; i < 1; i++) {

            int spawnX = getRandomInt(mX - 10, mX + 10);
            int spawnY = getRandomInt(mY - 10, mY + 10);

            if (!getTileRef(spawnX, spawnY).walkable) {
                attemptCount++;
                if (attemptCount >= maxAttempts) break;
                i--;
                continue;
            }

            entt::entity member = registry.create();

            registry.emplace<Position>(member, spawnX, spawnY);
            registry.emplace<Renderable>(member, L'Z', glm::vec3(1.0f, 1.0f, 1.0f));

            registry.emplace<Movable>(member, 0.25f);

            registry.emplace<SquadMemberComponent>(member, squadEntity);

            registry.emplace<Name>(member, "Zombie");
            registry.emplace<Hostile>(member);
            registry.emplace<Health>(member, 40);

            registry.emplace<Zombie>(member);
            mainWorld.objectManager.addObject(spawnX, spawnY, member);

            controller.members.push_back(member);
        }
    }

    auto& ui = Game::getInstance().getInGameUI();

    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >= 1.0) {
        fps = "FPS: " + std::to_string(nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
    }

    ui.update(fps);


    auto& i = Game::getInstance().getVillagerInfoUI();
    if (Game::getInstance().v_villager != entt::null) {
        i.update(Game::getInstance().v_villager);
        Game::getInstance().getUIManager().push(UI::Villager);
    }
    else {
        //Game::getInstance().getUIManager().remove(UI::Villager);
    }
	
}


void handleMode() {

    GameState& gameState = Game::getInstance().gameState;

    int left = std::min(gameState.placingState.corner.first, gameState.inputState.mouseTileX);
    int right = std::max(gameState.placingState.corner.first, gameState.inputState.mouseTileX);
    int top = std::min(gameState.placingState.corner.second, gameState.inputState.mouseTileY);
    int bottom = std::max(gameState.placingState.corner.second, gameState.inputState.mouseTileY);

    if (gameState.placingState.currentMode == Mode::BUILD) {
		build(left, right, top, bottom);
    }
    else if (gameState.placingState.currentMode == Mode::HARVEST) {
		harvest(left, right, top, bottom);
    }
    else if (gameState.placingState.currentMode == Mode::PLANT) {
		plant(left, right, top, bottom);
    }
    else if (gameState.placingState.currentMode == Mode::STOCKPILE) {
		stockpile(left, right, top, bottom);
	}
}

void handleClickedItem(int x, int y) {

    GameState& gameState = Game::getInstance().gameState;

    ObjectManager* manager = &mainWorld.objectManager;
    auto& uiManager = Game::getInstance().getUIManager();

	int mouseX = gameState.inputState.mouseTileX;
	int mouseY = gameState.inputState.mouseTileY;

	auto& itemList = manager->getObjectsAt(mouseX, mouseY);

    Game::getInstance().getUIManager().remove(UI::Stockpile);

    entt::entity clickedVillager = entt::null;
    for (auto& i : itemList) {
        if (auto j = mainWorld.registry.try_get<Skills>(i)) {
            clickedVillager = i;
            break;
        }
    }

    if (clickedVillager != entt::null) {
        Game::getInstance().v_villager = clickedVillager;
    } 
    else if (manager->has(x, y, "Carpentry Bench")) {
        uiManager.addOrRemoveFrame(UI::Carpentry);
    }
    else if (manager->has(x, y, "Stone Cutter")) {
        uiManager.addOrRemoveFrame(UI::StoneCutter);
    }
    else if (manager->has(x, y, "Anvil")) {
        uiManager.addOrRemoveFrame(UI::Anvil);
	}
    else if (manager->has(x, y, "Gun Bench")) {
        uiManager.addOrRemoveFrame(UI::Gun);
    }
    else if (manager->has(x, y, "Stockpile")) {
        auto s = mainWorld.getColony().atStockpile(x, y);
        Game::getInstance().getStockpileUI().updateStockpileUI(*s);
        uiManager.addOrRemoveFrame(UI::Stockpile);

        auto& frame = Game::getInstance().getStockpileUI();
        frame.infoPanel->setPosition(mouseX, mouseY);
    }
    else if (manager->has(x, y, "Furnace")) {
        uiManager.addOrRemoveFrame(UI::Furnace);
    }
    /*else if (!itemList.empty()) {
        Game::getInstance().getItemUI().configureItemFrame(mainWorld.registry, manager->getTopEntity(mouseX, mouseY));
        uiManager.addOrRemoveFrame(UI::Item);
    }*/
}

void addBlueprint(int x, int y, std::string item) {

    auto display = ObjectRegistry::getInstance().getStaticComponent<Renderable>(item);
    auto name = ObjectRegistry::getInstance().getStaticComponent<Name>(item);

    if (!display || !name) return;

	auto& registry = mainWorld.registry;
    auto blueprint = registry.create();

    registry.emplace<Position>(blueprint, x, y);
    registry.emplace<Name>(blueprint, name->name + " Blueprint");

    registry.emplace<Renderable>(blueprint, display->character, normalizeRGB(glm::vec3(0, 65, 186)));
    registry.emplace<BlueprintTag>(blueprint);

    mainWorld.objectManager.addObject(x, y, blueprint);
}

bool hasBlueprint(int x, int y) {
    auto& objects = mainWorld.objectManager.getObjectsAt(x, y);
    for (auto& obj : objects) {
        if (mainWorld.registry.all_of<BlueprintTag>(obj)) {
            return true;
        }
    }
    return false;
}

void build(int left, int right, int top, int bottom) {

    GameState& gameState = Game::getInstance().gameState;
    ObjectManager& manager = mainWorld.objectManager;

    std::string itemName = Game::getInstance().getBuildItem();

    int mX = gameState.inputState.mouseTileX;
    int mY = gameState.inputState.mouseTileY;

    if (mainWorld.placementMode == PlacementMode::SINGLE) {
        if (hasBlueprint(mX, mY)) {
            std::cout << "Tile (" << mX << ", " << mY << ") already has a blueprint. Skipping build job." << std::endl;
            return;
        }

        auto staticObject = ObjectRegistry::getInstance().getStaticComponent<Furniture>(itemName);
        if (staticObject) {
            auto loc = mainWorld.getColony().findUnclaimedItemInAllStockpile(itemName, mainWorld.registry);
            if (!loc) {
                gameState.placingState.placing = false;
                return;
            }
            
            if (!manager.hasComponent<Floor>(mX, mY, mainWorld.registry)) return;
            if (manager.hasComponent<Structure>(mX, mY, mainWorld.registry)) return;

            addBlueprint(mX, mY, itemName);
            mainWorld.registry.get<Claimable>(loc.value().item).claimed = true;

            Job* job = new BuildFurniture(entt::null, entt::null, SkillType::None, loc->item, loc->x, loc->y, mX, mY);
            job->priority = 30;
            JobManager::addJob(job);
        } else {

            if (!manager.isEmpty(mX, mY)) return;
			addBlueprint(mX, mY, itemName);

            Job* job = new Build(entt::null, entt::null, SkillType::Building, itemName, mX, mY);
            job->priority = 30;
            JobManager::addJob(job);
        }
    }

    else if (mainWorld.placementMode == PlacementMode::SQUARE) {
        for (int x = left; x <= right; x++) {
            for (int y = top; y <= bottom; y++) {
                if (x == left || x == right || y == top || y == bottom) {

                    if (hasBlueprint(x, y)) {
                        std::cout << "Tile (" << x << ", " << y << ") already has a blueprint. Skipping build job." << std::endl;
                        continue;
					}

                    if (!mainWorld.objectManager.isEmpty(x, y)) continue;

					addBlueprint(x, y, itemName);

                    Job* job = new Build(entt::null, entt::null, SkillType::Building, itemName, x, y);
                    job->priority = 30;
                    JobManager::addJob(job);
                }
            }
        }
    }
    else if (mainWorld.placementMode == PlacementMode::FILLED_SQUARE) {
        for (int x = left; x <= right; x++) {
            for (int y = top; y <= bottom; y++) {

                if (hasBlueprint(x, y)) {
                    std::cout << "Tile (" << x << ", " << y << ") already has a blueprint. Skipping build job." << std::endl;
                    continue;
                }

                if (!mainWorld.objectManager.isEmpty(x, y)) continue;

                addBlueprint(x, y, itemName);

                Job* job = new Build(entt::null, entt::null, SkillType::Building, itemName, x, y);
                job->priority = 30;
                JobManager::addJob(job);
            }
        }
    }
}

void harvest(int left, int right, int top, int bottom) {
    ObjectManager& manager = mainWorld.objectManager;
    auto& registry = mainWorld.registry;

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            Tile& tile = getTileRef(x, y);
            auto& objectList = manager.getObjectsAt(x, y);
            auto& topObject = objectList[0];

            if (manager.has(x, y, "Stockpile")) continue;
            if (objectList.empty()) continue;
			if (tile.markedForHarvest) continue;
            if (!mainWorld.getColony().isInsideColony(x, y, 75)) continue;

            if (auto* i = registry.try_get<Crop>(topObject)) {
                if (i->state == CropState::SPROUT) continue;
            }

            if (auto* i = registry.try_get<Harvestable>(topObject)) {
                tile.markedForHarvest = true;
                tile.anim.type = animType::RED_X;

                Job* job = new HarvestTile(entt::null, entt::null, i->requiredSkill, topObject, x, y);
                job->priority = 20;
                JobManager::JobList.push_back(job);
            }
            else if (registry.all_of<Pickupable>(topObject)) {
                registry.remove<Pickupable>(topObject);
                registry.emplace<NeedsMoving>(topObject);
            }
        }
    }
}

void plant(int left, int right, int top, int bottom) {
    ObjectManager& manager = mainWorld.objectManager;
    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            if (manager.has(x, y, "Stockpile")) continue;
            if (getTileRef(x, y).type != tileType::GRASS && getTileRef(x, y).type != tileType::SOIL) continue;

            auto itemLocation = mainWorld.getColony().findUnclaimedItemInAllStockpile(Game::getInstance().selectedPlantItem, mainWorld.registry);

            if (itemLocation) {

                mainWorld.registry.get<Claimable>(itemLocation->item).claimed = true;

                addBlueprint(x, y, Game::getInstance().selectedPlantItem);
                Job* job = new Plant(entt::null, entt::null, SkillType::Farming, Game::getInstance().selectedPlantItem, itemLocation.value(), x, y);
                job->priority = 30;
                JobManager::JobList.push_back(job);
            }
        }
    }
}

void stockpile(int left, int right, int top, int bottom) {

    ObjectManager& manager = mainWorld.objectManager;

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {
            Tile& tile = getTileRef(x, y);
            if (!mainWorld.getColony().isInsideColony(x, y, -1) || !tile.walkable || manager.has(x, y, "Stockpile")) {
                return;
			}
        }
    }

    int locX = std::min(left, right);
    int locY = std::min(top, bottom);

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            Tile& tile = getTileRef(x, y);
			tile.addObject(x, y, "Stockpile");
        }
    }

	int width = std::abs(right - left) + 1;
	int height = std::abs(bottom - top) + 1;
    
    Stockpile stockpile({ locX, locY }, width, height);
    mainWorld.getColony().addStockpile(stockpile);
}