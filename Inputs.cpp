#include "Inputs.h"
#include "Job.h"
#include "Item.h"
#include "Tile.h"
#include "HarvestRules.h"
#include "World.h"
#include "Game.h"
#include "UI.h"
#include "HeatEmitter.h"
#include "Save.h"
#include "Structure.h"

double lastTime = glfwGetTime();
int nbFrames = 0;
std::string fps;

float moveClock = 0.0f;

bool spacePressed = false;

Villager* viewing;
/*

┌──────────────────────────────┐
│┌─┐ Name : Dr.Coomer         ╳│
││☺│ Occupation : Miner        │
│└─┘                           │
│ Current Action : Mining Stone│
│                              │
│ Holding : 4x Rock            │
│ Tool : Stone Pickaxe         │
│                              │
│ Health : 100 / 100 		   │
│ Armor : 0 / 0 			   │
└──────────────────────────────┘

*/


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

        viewing = nullptr;

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

                for (auto& i : mainWorld.getAllVillagers()) {
                    if (i->xPos == mouseTileX && i->yPos == mouseTileY) {
                        viewing = i;
                        i->printJobQueue();
                        break;
                    }
                }
                handleClickedItem(mouseTileX, mouseTileY);


                //Game::getInstance().getLightManager().addLight(glm::vec2(mouseTileX, mouseTileY), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, 1.0f, -1.0f);
                //std::vector<float> map = Game::getInstance().getLightManager().BFSLight();
                //mainWorld.setLightMap(map);

				//Game::getInstance().getHeatManager().addHeatSource(mouseTileX, mouseTileY, 35.0f, 10.0f);
				//std::vector<float> heatMap = Game::getInstance().getHeatManager().calculateHeatMap(calculateMapSize());
				//mainWorld.setTemperatureMap(heatMap);
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

    /*if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        yTextSpacing++;
        yFrustum = scrHeight / yTextSpacing;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (yTextSpacing > 1) yTextSpacing--;
        yFrustum = scrHeight / yTextSpacing;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        xTextSpacing++;
        xFrustum = scrWidth / xTextSpacing;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (xTextSpacing > 1) xTextSpacing--;
        xFrustum = scrWidth / xTextSpacing;
    }*/

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
        save();
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
	ui.population->changeText(L"Population: " + std::to_wstring(mainWorld.getAllCreatures().size()));

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

    if (viewing) {
        auto& i = Game::getInstance().getVillagerInfoUI();
        std::string str = viewing->firstname + " " + viewing->lastname;
        i.text->changeText(std::wstring(str.begin(), str.end()));
        std::string jobStr = jobTypeToString(viewing->getJob());
		//i.job->changeText(std::wstring(jobStr.begin(), jobStr.end()));
        
        std::string inv = activityStateToString(viewing->activity_state);
        /*for (auto& i : viewing->inventory.inventory) {
            inv += i.first + " " + std::to_string(i.second);
        }*/
        i.inventory->changeText(std::wstring(inv.begin(), inv.end()));

        std::string health = std::to_string(viewing->health) + " / 100";
        std::string hunger = "Social: " + std::to_string(viewing->social);
        i.health->changeText(std::wstring(health.begin(), health.end()));
        i.hunger->changeText(std::wstring(hunger.begin(), hunger.end()));

		std::string skills = "Skills:|";
        for (auto& skill : viewing->skills) {
            skills += skillTypeToString(skill.first) + ": " + std::to_string(skill.second) + "|";
		}
        i.skills->changeText(std::wstring(skills.begin(), skills.end()));

        std::string log = "Log:|";

        for (auto& i : viewing->action_log) {
            log += i + "|";
        }

        i.log->changeText(std::wstring(log.begin(), log.end()));

        i.infoPanel->setSize(std::max(str.size(), jobStr.size()) + 8, 18);

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

        for (auto& item : tile.items) {
            itemStr += item->name;

            if (item->type == Type::Crop) {
                Crop* crop = static_cast<Crop*>(item.get());
                float growth = static_cast<float>(crop->growStage + 1) / static_cast<float>(crop->stages.size());
				//float growth = ((float)(crop->growStage + 1)) / ((float)crop->stages.size());

				growth = std::clamp(growth, 0.0f, 1.0f);

                int percent = static_cast<int>(growth * 100.0f);
                itemStr += " (" + std::to_string(percent) + "% Grown)";
            }

            if (item->type == Type::Tool) {
                Tool* tool = static_cast<Tool*>(item.get());
                itemStr += " (" + materialToString(tool->material) + ")";
            }

            if (item->type == Type::Structure) {
                Structure* structure = static_cast<Structure*>(item.get());
                itemStr += " " + std::to_string(structure->health);
            }

			itemStr += " " + std::to_string(item->claimed);

            itemStr += "|";
        }

        ui.tileItems->changeText(std::wstring(itemStr.begin(), itemStr.end()));
        std::string type = typeToString(tile.type) + " " + std::to_string(mainWorld.getTemperatureMapIndex(mouseTileX, mouseTileY));
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
    auto& uiManager = Game::getInstance().getUIManager();

    if (getTileRef(x, y).containsItem("Carpentry Bench")) {
        uiManager.addOrRemoveFrame(UI::Carpentry);
    }
    else if (getTileRef(x, y).containsItem("Stone Cutter")) {
        uiManager.addOrRemoveFrame(UI::StoneCutter);
    }
    else if (getTileRef(x, y).containsItem("Anvil")) {
        uiManager.addOrRemoveFrame(UI::Anvil);
	}
    else if (getTileRef(x, y).containsItem("Furnace")) {
        // do later
    }
    else if (getTileRef(x, y).containsItem("Gun Bench")) {
        uiManager.addOrRemoveFrame(UI::Gun);
    }
    else if (auto s = mainWorld.atStockpile(mouseTileX, mouseTileY)) {
        Game::getInstance().getStockpileUI().updateStockpileUI(*s);
        Game::getInstance().getUIManager().addOrRemoveFrame(UI::Stockpile);
        //mainWorld.removeStockpile(*s);
    }
}

void build(int left, int right, int top, int bottom) {

    auto item = Game::getInstance().getBuildItem();

    Game::getInstance().getFurnitureUI().configureFurnitureFrame();

    if (!item) return;

	std::cout << "Building: " << item->name << std::endl;
    std::string itemName = item->name;

    if (mainWorld.placementMode == PlacementMode::SINGLE) {
        std::cout << "Adding build job for " << itemName << " at (" << mouseTileX << ", " << mouseTileY << ")" << std::endl;
        if (item->type == Type::Furniture) {
            auto loc = mainWorld.findUnclaimedItemInAllStockpile(itemName);
            if (!loc) {
                placing = false;
                return;
            }
			loc->second->claimed = true;

            Job* job = new BuildFurniture(nullptr, nullptr, SkillType::None, item, loc->first.first, loc->first.second, mouseTileX, mouseTileY);
            job->priority = 30;
            JobManager::addJob(job);
        }
        else {
            Job* job = new Build(nullptr, nullptr, SkillType::Building, itemName, mouseTileX, mouseTileY);
            job->priority = 30;
            JobManager::addJob(job);
        }
    }

    else if (mainWorld.placementMode == PlacementMode::LINE) {
        auto line = bresenham(top, left, bottom, right);
        for (auto& i : line) {
            Job* job = new Build(nullptr, nullptr, SkillType::Building, itemName, i.first, i.second);
            job->priority = 30;
            JobManager::addJob(job);
        }
	}

    else if (mainWorld.placementMode == PlacementMode::SQUARE) {
        for (int x = left; x <= right; x++) {
            for (int y = top; y <= bottom; y++) {
                if (x == left || x == right || y == top || y == bottom) {
                    Job* job = new Build(nullptr, nullptr, SkillType::Building, itemName, x, y);
                    job->priority = 30;
                    JobManager::addJob(job);
                }
            }
        }
    }

}

void harvest(int left, int right, int top, int bottom) {

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            Tile& tile = getTileRef(x, y);

            if (mainWorld.atStockpile(x, y)) continue;
            if (tile.items.empty()) continue;
			if (tile.markedForHarvest) continue;


            if (tile.items[0]->type == Type::Crop) {
                auto crop = static_cast<Crop*>(tile.items[0].get());
                if (!crop->isGrown()) {
                    tile.markedForHarvest = false;
                    continue;
                }
            }

            Rule* rule = HarvestRuleRegistry::getInstance().get(tile.items[0]->name);
            if (!rule) {
                if (tile.items[0]->type == Type::Item || tile.items[0]->type == Type::Tool || tile.items[0]->type == Type::Food) {
                    mainWorld.addItemToMove(tile.items[0], x, y);
                    tile.markedForHarvest = false;
                    tile.items[0]->claimed = true;
                }
                continue;
			}


            SkillType skill = rule ? rule->skillType : SkillType::None;


            tile.markedForHarvest = true;
            tile.anim.type = animType::RED_X;

            if (rule->toolRequired == "None") {

                Job* job = new HarvestTile(nullptr, nullptr, skill, tile.items[0].get()->name, x, y);
                job->priority = 40;
                JobManager::JobList.push_back(job);
            }
            else {
                auto toolInRegistry = ObjectRegistry::getInstance().get(rule->toolRequired);
                Tool* tool = dynamic_cast<Tool*>(toolInRegistry.get());
                if (tool) {
                    //std::cout << "Adding harvest job for " << tile.items[0]->name << " at (" << x << ", " << y << ") with tool " << tool->name << std::endl;
                    Job* harvestJob = new HarvestTile(nullptr, tool, skill, tile.items[0].get()->name, x, y);
                    harvestJob->priority = 40;
                    JobManager::JobList.push_back(harvestJob);
                }
                else {
                    tile.markedForHarvest = false;
                    tile.anim.type = animType::NONE;
                    std::cerr << "Error: Configured tool '" << rule->toolRequired << "' failed cast logic.\n";
                    continue;
                }
            }		
        }
    }
}

void plant(int left, int right, int top, int bottom) {
    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {
            if (!mainWorld.atStockpile(x, y)) {
                Job* job = new Plant(nullptr, nullptr, SkillType::Farming, Game::getInstance().selectedPlantItem, x, y);
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

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {
            Tile& tile = getTileRef(x, y);
            if (!tile.walkable || tile.containsItem("Stockpile")) {
                std::cout << "Cannot create stockpile: Tile at (" << x << ", " << y << ") is not empty.\n";
                return;
			}
        }
    }

    auto stockpileItem = ObjectRegistry::getInstance().get("Stockpile");

    int locX = std::min(left, right);
    int locY = std::min(top, bottom);

    std::cout << "Creating stockpile at (" << locX << ", " << locY << ")\n";

    for (int x = left; x <= right; x++) {
        for (int y = top; y <= bottom; y++) {

            Tile& tile = getTileRef(x, y);

            tile.items.clear();
            tile.addObject("Stockpile");
        }
    }

	int width = std::abs(right - left) + 1;
	int height = std::abs(bottom - top) + 1;
    
    Stockpile stockpile({ locX, locY }, width, height);
    mainWorld.addStockpile(stockpile);
}