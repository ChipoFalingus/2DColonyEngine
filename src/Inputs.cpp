#include "Inputs.h"
#include "Jobs/Job.h"
#include "World/Tile.h"
#include "World/World.h"
#include "Game.h"
#include "UI/UI.h"
#include "Entities/ItemComponents.h"
#include "Entities/CreatureComponents.h"
#include "Entities/Squad.h"

double lastTime = glfwGetTime();
int nbFrames = 0;
std::string fps;

float moveClock = 0.0f;

bool spacePressed = false;

entt::entity viewing = entt::null;


void handleMode();
void handleClickedItem(int x, int y);
void build(int left, int right, int top, int bottom);
void harvest(int left, int right, int top, int bottom);
void plant(int left, int right, int top, int bottom);
void stockpile(int left, int right, int top, int bottom);


void processInput(GLFWwindow* window) {

    moveClock += Clock::deltaTime;

    if (moveClock > 0.01f) {
        moveClock = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            yPlayer--;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            yPlayer++;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            xPlayer--;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            xPlayer++;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !clicked) {

        viewing = entt::null;

        if (currentMode != Mode::NONE) {
            if (!placing) {
                corner = { mouseTileX, mouseTileY };
                placing = true;
            }
            else {
                handleMode();

                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
                {
                    placing = false;
                    currentMode = Mode::NONE;
                }
            }
        }
        else {

            if (mainWorld.isRendered()) {
                placing = false;
                handleClickedItem(mouseTileX, mouseTileY);
            }
        }

        clicked = true;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        clicked = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        placing = false;
        setMode(Mode::NONE);
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!viewMiniMap) {
			viewMiniMap = true;
        }
        else if (viewMiniMap) {
			viewMiniMap = false;
        }

        auto& UIManager = Game::getInstance().getUIManager();
		UIManager.swapFrame(UI::Minimap, UI::InGame);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!viewHeightMap) viewHeightMap = true;
        else if (viewHeightMap) viewHeightMap = false;
    }

    bool pressed = false;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !pressed) {
        enableWater = !enableWater;
		pressed = true;
    }
    else {
		pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		xPlayer = 0;
		yPlayer = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		viewUI = !viewUI;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        mainWorld.updateMiniMap();
        
    }

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        //save();
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

    auto& ui = Game::getInstance().getInGameUI();


	std::string dayStr = "Day: " + std::to_string(mainWorld.dayCycle.getDayNumber()) + "|" + timePeriodToString(mainWorld.dayCycle.getTimePeriod());
    ui.day->changeText(std::wstring(dayStr.begin(), dayStr.end()));

	//ui.playerPos->changeText(L"Player Position: (" + std::to_wstring(xPlayer) + L"," + std::to_wstring(yPlayer) + L")");
    ui.playerPos->changeText(L"Mouse Position: (" + std::to_wstring(mouseTileX) + L"," + std::to_wstring(mouseTileY) + L")");

    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >= 1.0) {
        fps = "FPS: " + std::to_string(nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
    }

	ui.FPS->changeText(std::wstring(fps.begin(), fps.end()));

    auto squadPop = mainWorld.registry.view<SquadMemberComponent>();
	ui.population->changeText(L"Squad Size: " + squadPop.size());

    int uiX = mouseTileX - (xPlayer - xFrustum / 2);
    int uiY = mouseTileY - (yPlayer - yFrustum / 2);

    if (placing && mainWorld.placementMode == PlacementMode::SQUARE) {
        std::string xStr = std::to_string(std::abs(corner.first - mouseTileX) + 1);
        std::string yStr = std::to_string(std::abs(corner.second - mouseTileY) + 1);
        std::string dim = xStr + "x" + yStr;

		ui.placingDims->changeText(std::wstring(dim.begin(), dim.end()));
		ui.placingDims->setPosition(uiX - (dim.size() >> 1), uiY - 1);
        ui.placingDims->setAnchorPosition(xFrustum, yFrustum);
    }
    else {
        ui.placingDims->changeText(L"");
    }

    if (viewing != entt::null) {
        auto& i = Game::getInstance().getVillagerInfoUI();

		auto& registry = mainWorld.registry;
		auto name = registry.try_get<Name>(viewing);
        std::string str = name ? name->name : "Unknown";
        i.text->changeText(std::wstring(str.begin(), str.end()));
        

		auto skill = registry.try_get<Skills>(viewing);
		std::string skills = "Skills:|";
        if (skill) {
            for (auto& s : skill->skills) {
                skills += skillTypeToString(s.first) + ": " + std::to_string(s.second) + "|";
            }
        }
        i.skills->changeText(std::wstring(skills.begin(), skills.end()));

        std::string activity = activityStateToString(mainWorld.registry.get<JobComponent>(viewing).activity_state);
        i.hunger->changeText(std::wstring(activity.begin(), activity.end()));

        /*std::string log = "Log:|";

        for (auto& i : viewing->action_log) {
            log += i + "|";
        }*/

        /*i.log->changeText(std::wstring(log.begin(), log.end()));

        i.infoPanel->setSize(std::max(str.size(), jobStr.size()) + 8, 18);*/

        Game::getInstance().getUIManager().push(UI::Villager);
    }
    else {
        if (Game::getInstance().getUIManager().hasFrame(UI::Villager)) {
            Game::getInstance().getUIManager().deleteFrame(UI::Villager);
        }
    }

    if (mainWorld.isRendered()) {
        Tile& tile = getTileRef(mouseTileX, mouseTileY);
        std::string itemStr = "";

        auto& list = mainWorld.objectManager.getObjectsAt(mouseTileX, mouseTileY);
        auto& registry = mainWorld.registry;

        for (entt::entity e : list) {

            if (registry.valid(e) && registry.all_of<Name>(e)) {
                itemStr += registry.get<Name>(e).name + " (ObjectManager)|";
            }
        }

		/*auto view = mainWorld.registry.view<Name, Position>();

        for (auto [entity, name, position] : view.each()) {
            if (position.x == mouseTileX && position.y == mouseTileY) {
                itemStr += name.name + " (Registry)|";
            }
        }*/

        ui.tileItems->changeText(std::wstring(itemStr.begin(), itemStr.end()));
        std::string type = typeToString(tile.type);
        ui.tileType->changeText(std::wstring(type.begin(), type.end()));

        std::wstring lightLevel = std::wstring(L"Altitude: " + std::to_wstring(getTileRef(mouseTileX, mouseTileY).altitude));

        ui.waterLevel->changeText(lightLevel);
    }
	
}


void handleMode() {

    int left = std::min(corner.first, mouseTileX);
    int right = std::max(corner.first, mouseTileX);
    int top = std::min(corner.second, mouseTileY);
    int bottom = std::max(corner.second, mouseTileY);

    if (currentMode == Mode::BUILD) {
		build(left, right, top, bottom);
    }
    else if (currentMode == Mode::HARVEST) {
		harvest(left, right, top, bottom);
    }
    else if (currentMode == Mode::PLANT) {
		plant(left, right, top, bottom);
    }
    else if (currentMode == Mode::STOCKPILE) {
		stockpile(left, right, top, bottom);
	}
}

void handleClickedItem(int x, int y) {
    ObjectManager* manager = &mainWorld.objectManager;
    auto& uiManager = Game::getInstance().getUIManager();


    for (auto& i : manager->getObjectsAt(mouseTileX, mouseTileY)) {
        if (auto j = mainWorld.registry.try_get<Skills>(i)) {
            viewing = i;
            //i->printJobQueue();
            break;
        }
    }

    if (manager->has(x, y, "Carpentry Bench")) {
        uiManager.addOrRemoveFrame(UI::Carpentry);
    }
    else if (manager->has(x, y, "Stone Cutter")) {
        uiManager.addOrRemoveFrame(UI::StoneCutter);
    }
    else if (manager->has(x, y, "Anvil")) {
        uiManager.addOrRemoveFrame(UI::Anvil);
	}
    else if (manager->has(x, y, "Furnace")) {
        // do later
    }
    else if (manager->has(x, y, "Gun Bench")) {
        uiManager.addOrRemoveFrame(UI::Gun);
    }
    else if (manager->has(x, y, "Stockpile")) {
        auto s = mainWorld.atStockpile(x, y);
        Game::getInstance().getStockpileUI().updateStockpileUI(*s);
        Game::getInstance().getUIManager().addOrRemoveFrame(UI::Stockpile);
    }
}

void build(int left, int right, int top, int bottom) {

    std::string itemName = Game::getInstance().getBuildItem();

	std::cout << "Building: " << itemName << std::endl;

    if (mainWorld.placementMode == PlacementMode::SINGLE) {
        std::cout << "Adding build job for " << itemName << " at (" << mouseTileX << ", " << mouseTileY << ")" << std::endl;
        auto staticObject = ObjectRegistry::getInstance().getStaticObject(itemName);
        if (ObjectRegistry::getInstance().getStaticRegistry().all_of<Furniture>(staticObject)) {
            std::cout << itemName << " IS furniture" << std::endl;
            auto loc = mainWorld.findUnclaimedItemInAllStockpile(itemName);
            if (!loc) {
                placing = false;
                return;
            }

            Job* job = new BuildFurniture(entt::null, entt::null, SkillType::None, loc->second, loc->first.first, loc->first.second, mouseTileX, mouseTileY);
            job->priority = 30;
            JobManager::addJob(job);
        } else {
            std::cout << itemName << " IS NOT furniture" << std::endl;
            Job* job = new Build(entt::null, entt::null, SkillType::Building, itemName, mouseTileX, mouseTileY);
            job->priority = 30;
            JobManager::addJob(job);
        }
    }

    /*else if (mainWorld.placementMode == PlacementMode::LINE) {
        auto line = bresenham(top, left, bottom, right);
        for (auto& i : line) {
            Job* job = new Build(entt::null, entt::null, SkillType::Building, itemName, i.first, i.second);
            job->priority = 30;
            JobManager::addJob(job);
        }
	}*/

    else if (mainWorld.placementMode == PlacementMode::SQUARE) {
        for (int x = left; x <= right; x++) {
            for (int y = top; y <= bottom; y++) {
                if (x == left || x == right || y == top || y == bottom) {
                    Job* job = new Build(entt::null, entt::null, SkillType::Building, itemName, x, y);
                    job->priority = 30;
                    JobManager::addJob(job);
                }
            }
        }
    }

}

void harvest(int left, int right, int top, int bottom) {
    ObjectManager* manager = &mainWorld.objectManager;

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            Tile& tile = getTileRef(x, y);
            ObjectManager* manager = &mainWorld.objectManager;
            auto& objectList = manager->getObjectsAt(x, y);

            if (manager->has(x, y, "Stockpile")) continue;
            if (objectList.empty()) continue;
			if (tile.markedForHarvest) continue;

            if (auto* i = mainWorld.registry.try_get<Harvestable>(objectList[0])) {
                tile.markedForHarvest = true;
                tile.anim.type = animType::RED_X;
                
				std::cout << "Adding harvest job for " << mainWorld.registry.get<Name>(objectList[0]).name << " at (" << x << ", " << y << ")" << std::endl;

				std::cout << "Required skill: " << skillTypeToString(i->requiredSkill) << std::endl;

                Job* job = new HarvestTile(entt::null, entt::null, i->requiredSkill, objectList[0], x, y);
                job->priority = 40;
                JobManager::JobList.push_back(job);

            }
            else {
				std::cout << "Selected item is not harvestable. Cannot harvest at (" << x << ", " << y << ").\n";
            }
        }
    }
}

void plant(int left, int right, int top, int bottom) {
    ObjectManager* manager = &mainWorld.objectManager;
    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {
            if (!manager->has(x, y, "Stockpile")) {
                Job* job = new Plant(entt::null, entt::null, SkillType::Farming, Game::getInstance().selectedPlantItem, x, y);
                job->priority = 30;
			    JobManager::JobList.push_back(job);
            }
            else {
				std::cout << "Selected item is not a crop. Cannot plant at (" << x << ", " << y << ").\n";
            }
        }
    }
}

void stockpile(int left, int right, int top, int bottom) {

    ObjectManager* manager = &mainWorld.objectManager;

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {
            Tile& tile = getTileRef(x, y);
           if (/*!tile.walkable ||*/ manager->has(x, y, "Stockpile")) {
                std::cout << "Cannot create stockpile: Tile at (" << x << ", " << y << ") is not empty.\n";
                return;
			}
        }
    }

    int locX = std::min(left, right);
    int locY = std::min(top, bottom);

    std::cout << "Creating stockpile at (" << locX << ", " << locY << ")\n";

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            Tile& tile = getTileRef(x, y);

			tile.addObject_Clear(x, y, "Stockpile");
        }
    }

	int width = std::abs(right - left) + 1;
	int height = std::abs(bottom - top) + 1;
    
    Stockpile stockpile({ locX, locY }, width, height);
    mainWorld.addStockpile(stockpile);
}