#include "UI.h"
#include "World/World.h"
#include "Game.h"
#include "Jobs/JobType.h"
#include "Jobs/Job.h"

#include <thread>
#include <GLFW/glfw3.h>


// There's probably a better way to format this but its miles better than the nested stuff i had before

std::unordered_map<std::string, int> getFurnitureList();

void setupBuildButton(auto& button, std::string item, PlacementMode mode = PlacementMode::SQUARE) {
    button->setClickFunction([item, mode]() {
        setMode(Mode::BUILD);
        mainWorld.placementMode = mode;
        Game::getInstance().setBuildItem(item);
        });

    button->setHoverFunction([item]() {
        auto& frame = Game::getInstance().getInfoUI();
        frame.configureInfoFrame(item);
        Game::getInstance().getUIManager().push(UI::Info);
        });
}

// First menu when opening the game

MainMenuUI getMainMenuFrame() {
    MainMenuUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

	ui.title = &frame->addElement<Text>(0, -10, L"Fling's Colony", Anchor::CENTER);

    GameState& gameState = Game::getInstance().gameState;

    int xFrustum = gameState.cameraState.xFrustum;
    int yFrustum = gameState.cameraState.yFrustum;

	std::cout << ui.title->getAnchorPosition(xFrustum, yFrustum).first << ", " << ui.title->getAnchorPosition(xFrustum, yFrustum).second << std::endl;

    ui.new_game = &createButton(*frame, 0, 0, L"           New Game           ", Anchor::CENTER);

    ui.new_game->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame(UI::Main, UI::WorldSettings);
        });

    ui.load = &createButton(*frame, 0, 3, L"           Load Game          ", Anchor::CENTER);
    ui.settingsButton = &createButton(*frame, 0, 6, L"           Settings           ", Anchor::CENTER);

    ui.settingsButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
		uiManager.swapFrame(UI::Main, UI::Settings);
		});

    ui.exit = &createButton(*frame, 0, 9, L"             Exit             ", Anchor::CENTER);

	ui.backers = &createButton(*frame, 0, 0, L" Backers ", Anchor::BOTTOM_RIGHT);

	ui.text = &frame->addElement<Text>(0, 0, L"Created by Fling's Studio", Anchor::BOTTOM_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

SettingsUI getSettingsFrame() {
    SettingsUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.text = &frame->addElement<Text>(0, 1, L"Settings Menu", Anchor::TOP_CENTER);
	ui.back = &createButton(*frame, 0, 0, L" < Back ", Anchor::TOP_LEFT);

    ui.back->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame(UI::Settings, UI::Main);
		});

    ui.v_sync = &frame->addElement<Checkbox>(5, 5, "V-Sync", Anchor::TOP_LEFT);

    int SLIDER_MIN_VALUE = 1;
    int SLIDER_MAX_VALUE = 25;
    int START_POSITION = 5;
    int SEGMENTS = 25;
    
    ui.font_size = &frame->addElement<Slider>(5, 7, SLIDER_MIN_VALUE, SLIDER_MAX_VALUE, START_POSITION, SEGMENTS, false, "Font Size 24", Anchor::TOP_LEFT);

    SLIDER_MIN_VALUE = 0;
    SLIDER_MAX_VALUE = 50;
    START_POSITION = 16;
    SEGMENTS = 26;

    ui.x_text_spacing = &frame->addElement<Slider>(5, 9, SLIDER_MIN_VALUE, SLIDER_MAX_VALUE, START_POSITION, SEGMENTS, false, "X Text Spacing 24", Anchor::TOP_LEFT);

    START_POSITION = 22;

    ui.y_text_spacing = &frame->addElement<Slider>(5, 11, SLIDER_MIN_VALUE, SLIDER_MAX_VALUE, START_POSITION, SEGMENTS, false, "Y Text Spacing 24", Anchor::TOP_LEFT);

    SLIDER_MIN_VALUE = 50;
    SLIDER_MAX_VALUE = 150;
    START_POSITION = 100;
    SEGMENTS = 21;

    ui.camera_speed = &frame->addElement<Slider>(5, 13, SLIDER_MIN_VALUE, SLIDER_MAX_VALUE, START_POSITION, SEGMENTS, false, "Camera Speed 100", Anchor::TOP_LEFT);

    ui.apply = &createButton(*frame, 5, 15, L" Apply Changes ", Anchor::TOP_LEFT);

    ui.apply->setClickFunction([ui]() {
        Settings settings;
        settings.font_size = (float)ui.font_size->getValue() / 5;
        settings.xTextSpacing = ui.x_text_spacing->getValue();
        settings.yTextSpacing = ui.y_text_spacing->getValue();
        settings.camera_speed = ui.camera_speed->getValue();

        Game::getInstance().getSettingsManager().update(settings);
        GameState& gameState = Game::getInstance().gameState;

        gameState.cameraState.xFrustum = gameState.cameraState.scrWidth / Game::getInstance().getSettingsManager().get().xTextSpacing;
        gameState.cameraState.yFrustum = gameState.cameraState.scrHeight / Game::getInstance().getSettingsManager().get().yTextSpacing;
        std::cout << "Tile dimesions resized to " << gameState.cameraState.xFrustum << "x" << gameState.cameraState.yFrustum << std::endl;

        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.resize(gameState.cameraState.xFrustum, gameState.cameraState.yFrustum);
        for (auto& i : uiManager.getAllFrames()) {
            auto frame = i.second.get();
            frame->resize(gameState.cameraState.xFrustum, gameState.cameraState.yFrustum);

        }
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

void SettingsUI::update() {
    font_size->changeText("Font Size " + std::to_string(font_size->getValue()));
    x_text_spacing->changeText("X Text Spacing " + std::to_string(x_text_spacing->getValue()) + "px");
    y_text_spacing->changeText("Y Text Spacing " + std::to_string(y_text_spacing->getValue()) + "px");
    camera_speed->changeText("Camera Speed " + std::to_string(camera_speed->getValue()) + " tiles/s");

    glfwSwapInterval((v_sync->getChecked()) ? 1 : 0);
}

WorldSettingsUI getWorldSettingsFrame() {
    WorldSettingsUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.text = &frame->addElement<Text>(0, 1, L"World Settings Menu", Anchor::TOP_CENTER);

    ui.back = &createButton(*frame, 0, 0, L" < Back ", Anchor::TOP_LEFT);

    ui.back->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame(UI::WorldSettings, UI::Main);
        });

    GameState& gameState = Game::getInstance().gameState;

    int xFrustum = gameState.cameraState.xFrustum;
    int yFrustum = gameState.cameraState.yFrustum;

    ui.panel = &frame->addElement<Panel>(0, 3, xFrustum, yFrustum - 7, Anchor::TOP_CENTER);

    ui.begin = &createButton(*frame, -1, -1, L" Begin! ", Anchor::BOTTOM_RIGHT);

    ui.begin->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame(UI::WorldSettings, UI::Loading);

        mainWorld.getChunks().clear();
        mainWorld.generateWorld();

        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

LoadingUI getLoadingFrame() {
    LoadingUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    GameState& gameState = Game::getInstance().gameState;

    int xFrustum = gameState.cameraState.xFrustum;
    int yFrustum = gameState.cameraState.yFrustum;

    ui.panel = &frame->addElement<Panel>(0, 0, xFrustum, yFrustum, Anchor::TOP_CENTER);

    ui.text = &frame->addElement<Text>(1, -2, L"Loading...", Anchor::CENTER_LEFT);

    int numChunks = (calculateMapSize() * calculateMapSize()) / (chunkDim * chunkDim);
    ui.chunks = &frame->addElement<Text>(1, -1, L"0/" + std::to_wstring(numChunks), Anchor::CENTER_LEFT);

    ui.animation = &frame->addElement<Text>(1, 0, L"", Anchor::CENTER_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Menu you see when you're in the game
InGameUI getInGameFrame() {
    InGameUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    Panel& infoPanel = frame->addElement<Panel>(0, 0, 30, 6, Anchor::TOP_LEFT);
    Panel& tileInfoPanel = frame->addElement<Panel>(30, 0, 30, 5, Anchor::TOP_LEFT);

    ui.playerPos = &infoPanel.addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
    ui.FPS = &infoPanel.addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);
    ui.population = &infoPanel.addElement<Text>(1, 3, L"", Anchor::TOP_LEFT);
    ui.waterLevel = &infoPanel.addElement<Text>(1, 4, L"", Anchor::TOP_LEFT);

    ui.tileType = &tileInfoPanel.addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
    ui.tileItems = &tileInfoPanel.addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);

    ui.placingDims = &frame->addElement<Text>(0, 0, L"", Anchor::TOP_LEFT);
    ui.villagerName = &frame->addElement<Text>(0, 0, L"", Anchor::TOP_CENTER);

	ui.day = &frame->addElement<Text>(-7, -1, L"", Anchor::BOTTOM_RIGHT);

    ui.buildButton = &createButton(*frame, 0, 0, L"     ─█ Build     ", Anchor::BOTTOM_LEFT);

    ui.buildButton->setClickFunction([]() {
        setMode(Mode::NONE);
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Production);
        uiManager.remove(UI::Temperature);
        uiManager.addOrRemoveFrame(UI::Build);
        uiManager.remove(UI::Info);
        });

    ui.harvestButton = &createButton(*frame, 20, 0, L"    /♣ Harvest    ", Anchor::BOTTOM_LEFT);

    ui.harvestButton->setClickFunction([]() {
        mainWorld.placementMode = PlacementMode::SQUARE;
        setMode(Mode::HARVEST);
        });

    ui.plantButton = &createButton(*frame, 40, 0, L"     W. Plant     ", Anchor::BOTTOM_LEFT);

    ui.plantButton->setClickFunction([]() {
        setMode(Mode::NONE);
        auto& uiManager = Game::getInstance().getUIManager();
		Game::getInstance().getPlantUI().configurePlantFrame();
		uiManager.addOrRemoveFrame(UI::Plant);
        });

    ui.stockpileButton = &createButton(*frame, 60, 0, L"   == Stockpile   ", Anchor::BOTTOM_LEFT);

    ui.stockpileButton->setClickFunction([]() {
        mainWorld.placementMode = PlacementMode::FILLED_SQUARE;
        setMode(Mode::STOCKPILE);
        });

    ui.colony_info = &createButton(*frame, -3, 0, L" Info ", Anchor::TOP_RIGHT);

	ui.villagers = &createButton(*frame, 0, 0, L"☻", Anchor::TOP_RIGHT);

    ui.villagers->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
		//Game::getInstance().getVillagerListUI().configureVillagerList();
        uiManager.addOrRemoveFrame(UI::VillagerList);
		});

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

MiniMapUI getMiniMapFrame() {
    MiniMapUI ui;
    auto frame = std::make_unique<Frame>();

    frame->setType(ui.type);
    ui.text = &frame->addElement<Text>(0, 0, L"Mini Map Mode", Anchor::TOP_LEFT);
    ui.seed = &frame->addElement<Text>(0, 1, L"Seed: " + std::to_wstring(seed), Anchor::TOP_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

VillagerListUI getVillagerListFrame() {
    VillagerListUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    //ui.text = &frame->addElement<Text>(0, 0, L"Villagers", Anchor::TOP_LEFT);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Pop-up menu that shows up when you select "Build"
BuildUI getBuildFrame() {
    BuildUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.structureButton = &createButton(*frame, 0, -17, L" Structure ", Anchor::BOTTOM_LEFT);

    ui.structureButton->setClickFunction([&]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Structure);

        uiManager.remove(UI::Furniture);
        uiManager.remove(UI::Production);
        });


    ui.furnitureButton = &createButton(*frame, 0, -14, L" Furniture ", Anchor::BOTTOM_LEFT);

    ui.furnitureButton->setClickFunction([&]() {

        auto& furnitureFrame = Game::getInstance().getFurnitureUI();
        furnitureFrame.configureFurnitureFrame();

        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Furniture);

        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Production);
		});

    ui.productionButton = &createButton(*frame, 0, -11, L"Production ", Anchor::BOTTOM_LEFT);

    ui.productionButton->setClickFunction([&]() {
        mainWorld.placementMode = PlacementMode::SINGLE;
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Production);

        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Furniture);
        });

    ui.securityButton = &createButton(*frame, 0, -8, L" Security  ", Anchor::BOTTOM_LEFT);

    ui.temperatureButton = &createButton(*frame, 0, -5, L"Temperature", Anchor::BOTTOM_LEFT);

    ui.temperatureButton->setClickFunction([&]() {
        mainWorld.placementMode = PlacementMode::SINGLE;
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Temperature);

        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Furniture);
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

ProductionUI getProductionFrame() {
    ProductionUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.carpentry_bench = &createButton(*frame, 14, -17, L" Carpentry Bench ", Anchor::BOTTOM_LEFT);
	ui.stone_cutter = &createButton(*frame, 14, -14, L" Stone Cutter ", Anchor::BOTTOM_LEFT);
	ui.furnace = &createButton(*frame, 14, -11, L" Furnace ", Anchor::BOTTOM_LEFT);
    ui.anvil = &createButton(*frame, 14, -8, L" Anvil ", Anchor::BOTTOM_LEFT);
    ui.gun_bench = &createButton(*frame, 14, -5, L" Gun Bench ", Anchor::BOTTOM_LEFT);

    setupBuildButton(ui.carpentry_bench, "Carpentry Bench");
    setupBuildButton(ui.stone_cutter, "Stone Cutter");
    setupBuildButton(ui.furnace, "Furnace");
    setupBuildButton(ui.anvil, "Anvil");
    setupBuildButton(ui.gun_bench, "Gun Bench");

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Pop-up menu that shows up when you select "Structure" in the Build menu
StructureUI getStructureFrame() {
    StructureUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);


    ui.wood_wall = &createButton(*frame, 14, -17, L" Wooden Wall ", Anchor::BOTTOM_LEFT);
    ui.stone_wall = &createButton(*frame, 14, -14, L" Stone Wall ", Anchor::BOTTOM_LEFT);
    ui.wood_fence = &createButton(*frame, 14, -11, L" Wooden Floor ", Anchor::BOTTOM_LEFT);
    ui.stone_fence = &createButton(*frame, 14, -8, L" Stone Floor ", Anchor::BOTTOM_LEFT);
    ui.wood_floor = &createButton(*frame, 14, -5, L" ? ", Anchor::BOTTOM_LEFT);

    setupBuildButton(ui.wood_wall, "Wooden Wall");
    setupBuildButton(ui.stone_wall, "Stone Wall");
    setupBuildButton(ui.wood_fence, "Wooden Floor", PlacementMode::FILLED_SQUARE);
    setupBuildButton(ui.stone_fence, "Stone Floor", PlacementMode::FILLED_SQUARE);
    setupBuildButton(ui.wood_floor, "Wooden Wall");


    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

TemperatureUI getTemperatureFrame() {
    TemperatureUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.firepit = &createButton(*frame, 14, -5, L"Fire Pit", Anchor::BOTTOM_LEFT);
    ui.torch = &createButton(*frame, 14, -8, L"Torch", Anchor::BOTTOM_LEFT);

    setupBuildButton(ui.firepit, "Fire Pit");
    setupBuildButton(ui.torch, "Torch");

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

FurnitureUI getFurnitureFrame() {
    FurnitureUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
	ui.text = &frame->addElement<Text>(15, -5, L"Select furniture to place:", Anchor::BOTTOM_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

void FurnitureUI::configureFurnitureFrame() {
    for (auto& button : buttons) {
        Game::getInstance().getUIManager().getFrame(UI::Furniture)->removeElement(button);
	}

    buttons.clear();

    auto i = getFurnitureList();

    if (i.empty()) {
		text->changeText(L"No furniture to place");
    }
    else {
		text->changeText(L"");
    }

    int offset = 0;
    for (auto& [string, num] : i) {
        std::string j = string + " x" + std::to_string(num);
        auto frame = Game::getInstance().getUIManager().getFrame(UI::Furniture);
        auto button = &createButton(*frame,
            14, -5 + offset * 3,
            std::wstring(j.begin(), j.end()), Anchor::BOTTOM_LEFT);


        button->setClickFunction([this, string]() {
            setMode(Mode::BUILD);
            mainWorld.placementMode = PlacementMode::SINGLE;
            std::cout << string << std::endl;
            Game::getInstance().setBuildItem(string);
            Game::getInstance().getUIManager().remove(UI::Furniture);
            Game::getInstance().gameState.placingState.placing = true;
            });

        GameState& gameState = Game::getInstance().gameState;

        int xFrustum = gameState.cameraState.xFrustum;
        int yFrustum = gameState.cameraState.yFrustum;

        button->setAnchorPosition(xFrustum, yFrustum);
        buttons.push_back(button);
        offset--;
    }
}

std::unordered_map<std::string, int> getFurnitureList() {

    std::unordered_map<std::string, int> result;
    auto& registry = mainWorld.registry;

    auto& stockpiles = mainWorld.getStockpiles();
    for (auto& s : stockpiles) {
        for (auto& [loc, f] : s.getItems()) {
            for (auto& item : f) {

                bool furniture = registry.all_of<Furniture>(item);
                Claimable* claimable = registry.try_get<Claimable>(item);
                if (furniture && (!claimable || !claimable->claimed)) {
                    std::string name = mainWorld.registry.get<Name>(item).name;
                    result[name]++;
                }
            }
        }
    }

    return result;
}

CarpentryBenchUI getCarpentryBenchFrame() {
    CarpentryBenchUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_LEFT);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);
	ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_LEFT);

    int num = 0;

    auto& registry = ObjectRegistry::getInstance().getStaticRegistry();
    auto view = registry.view<Craftable>();

    for (auto i : view) {
        auto& name = registry.get<Name>(i);
        const auto& recipe = view.get<Craftable>(i);

        if (recipe.benchRequired != "Carpentry Bench") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(name.name.begin(), name.name.end()), Anchor::TOP_LEFT);


        button->setClickFunction([name] {
            Job* job = new Craft(entt::null, entt::null, SkillType::Carpentry, name.name);
            job->priority = 25;
            JobManager::addJob(job);
            });

        button->setHoverFunction([ui, recipe, name] {
			std::string infoStr = "Ingredients:|";
            for (auto& j : recipe.ingredients) {
                std::string ingredientStr = j.first + ": x" + std::to_string(j.second);
				infoStr += ingredientStr + "|";
				
            }
            infoStr += "|";
            infoStr += "Produces:|";
			infoStr +=  "x" + std::to_string(recipe.quantity) + " " + name.name;
			ui.ingredients->changeText(std::wstring(infoStr.begin(), infoStr.end()));
			});

        GameState& gameState = Game::getInstance().gameState;

        int xFrustum = gameState.cameraState.xFrustum;
        int yFrustum = gameState.cameraState.yFrustum;

		button->setAnchorPosition(xFrustum, yFrustum);

        ui.craftable_items.push_back(button);
        num++;
    }

	ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Carpentry);
		});

    ui.panel->setSize(35, std::max(num * 3 + 3, 5));

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

StoneCutterUI getStoneCutterFrame() {
    StoneCutterUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_LEFT);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);
    ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_LEFT);

    int num = 0;

    auto& registry = ObjectRegistry::getInstance().getStaticRegistry();
    auto view = registry.view<Craftable>();

    for (auto i : view) {
        auto& name = registry.get<Name>(i);
        const auto& recipe = view.get<Craftable>(i);

        if (recipe.benchRequired != "Stone Cutter") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(name.name.begin(), name.name.end()), Anchor::TOP_LEFT);


        button->setClickFunction([name] {
            Job* job = new Craft(entt::null, entt::null, SkillType::Masonry, name.name);
            job->priority = 25;
            JobManager::addJob(job);
            });

        button->setHoverFunction([ui, recipe, name] {
            std::string infoStr = "Ingredients:|";
            for (auto& j : recipe.ingredients) {
                std::string ingredientStr = j.first + ": x" + std::to_string(j.second);
                infoStr += ingredientStr + "|";

            }
            infoStr += "|";
            infoStr += "Produces:|";
            infoStr += "x" + std::to_string(recipe.quantity) + " " + name.name;
            ui.ingredients->changeText(std::wstring(infoStr.begin(), infoStr.end()));
            });

        GameState& gameState = Game::getInstance().gameState;

        int xFrustum = gameState.cameraState.xFrustum;
        int yFrustum = gameState.cameraState.yFrustum;

        button->setAnchorPosition(xFrustum, yFrustum);

        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::StoneCutter);
        });

    ui.panel->setSize(35, std::max(num * 3 + 3, 5));

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

AnvilUI getAnvilFrame() {
    AnvilUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_LEFT);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);
    ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_LEFT);

    int num = 0;

    auto& registry = ObjectRegistry::getInstance().getStaticRegistry();
    auto view = registry.view<Craftable>();

    for (auto i : view) {
        auto& name = registry.get<Name>(i);
        const auto& recipe = view.get<Craftable>(i);

        if (recipe.benchRequired != "Anvil") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(name.name.begin(), name.name.end()), Anchor::TOP_LEFT);


        button->setClickFunction([name] {
            Job* job = new Craft(entt::null, entt::null, SkillType::Masonry, name.name);
            job->priority = 25;
            JobManager::addJob(job);
            });

        button->setHoverFunction([ui, recipe, name] {
            std::string infoStr = "Ingredients:|";
            for (auto& j : recipe.ingredients) {
                std::string ingredientStr = j.first + ": x" + std::to_string(j.second);
                infoStr += ingredientStr + "|";

            }
            infoStr += "|";
            infoStr += "Produces:|";
            infoStr += "x" + std::to_string(recipe.quantity) + " " + name.name;
            ui.ingredients->changeText(std::wstring(infoStr.begin(), infoStr.end()));
            });

        GameState& gameState = Game::getInstance().gameState;

        int xFrustum = gameState.cameraState.xFrustum;
        int yFrustum = gameState.cameraState.yFrustum;

        button->setAnchorPosition(xFrustum, yFrustum);

        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Anvil);
        });

    ui.panel->setSize(35, std::max(num * 3 + 3, 5));

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

GunBenchUI getGunBenchFrame() {
    GunBenchUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_LEFT);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);
    ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_LEFT);

    int num = 0;

    auto& registry = ObjectRegistry::getInstance().getStaticRegistry();
    auto view = registry.view<Craftable>();

    for (auto i : view) {
        auto& name = registry.get<Name>(i);
        const auto& recipe = view.get<Craftable>(i);

        if (recipe.benchRequired != "Gun Bench") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(name.name.begin(), name.name.end()), Anchor::TOP_LEFT);


        button->setClickFunction([name] {
            Job* job = new Craft(entt::null, entt::null, SkillType::Masonry, name.name);
            job->priority = 25;
            JobManager::addJob(job);
            });

        button->setHoverFunction([ui, recipe, name] {
            std::string infoStr = "Ingredients:|";
            for (auto& j : recipe.ingredients) {
                std::string ingredientStr = j.first + ": x" + std::to_string(j.second);
                infoStr += ingredientStr + "|";

            }
            infoStr += "|";
            infoStr += "Produces:|";
            infoStr += "x" + std::to_string(recipe.quantity) + " " + name.name;
            ui.ingredients->changeText(std::wstring(infoStr.begin(), infoStr.end()));
            });

        GameState& gameState = Game::getInstance().gameState;

        int xFrustum = gameState.cameraState.xFrustum;
        int yFrustum = gameState.cameraState.yFrustum;

        button->setAnchorPosition(xFrustum, yFrustum);

        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Gun);
        });

    ui.panel->setSize(35, std::max(num * 3 + 3, 5));

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}


StockpileUI getStockpileFrame() {
    StockpileUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.infoPanel = &frame->addElement<Panel>(0, 14, 6, 7, Anchor::TOP_LEFT );
    //ui.text = &ui.infoPanel->addElement<Text>(20, 1, L"Filters:", Anchor::TOP_LEFT);
    ui.capacity = &ui.infoPanel->addElement<Text>(40, 2, L"", Anchor::TOP_LEFT);
    ui.contents = &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

void StockpileUI::updateStockpileUI(Stockpile& stockpile) {
    for (auto& checkbox : typeFilters) {
        Game::getInstance().getUIManager().getFrame(UI::Stockpile)->removeElement(checkbox);
	}
    typeFilters.clear();
    //std::wstring capacityStr = L"Capacity: " + std::to_wstring(stockpile.getHeight() * stockpile.getWidth());

    //capacity->changeText(capacityStr);
    std::wstring contentsStr = L"Contents:||";

	auto items = stockpile.getItemCounts();

    for (auto& [name, count] : items) {
        contentsStr += std::wstring(name.begin(), name.end()) + L": x" + std::to_wstring(count) + L"|";
	}

	infoPanel->setSize(20, items.size() + 4);

    contents->changeText(contentsStr);
}

VillagerInfoUI getVillagerInfoFrame() {
    VillagerInfoUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.infoPanel = &frame->addElement<Panel>(0, 10, 19, 19, Anchor::TOP_LEFT);

    ui.text = &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
	ui.job = &ui.infoPanel->addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);
    ui.inventory = &ui.infoPanel->addElement<Text>(1, 3, L"", Anchor::TOP_LEFT);
    ui.health = &ui.infoPanel->addElement<Text>(1, 4, L"", Anchor::TOP_LEFT);
    ui.hunger = &ui.infoPanel->addElement<Text>(1, 5, L"", Anchor::TOP_LEFT);

	ui.skills = &ui.infoPanel->addElement<Text>(1, 6, L"", Anchor::TOP_LEFT);

    ui.log = &ui.infoPanel->addElement<Text>(20, 1, L"", Anchor::TOP_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

HarvestUI getHarvestFrame() {
    HarvestUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    
	ui.infoPanel = &frame->addElement<Panel>(0, 10, 6, 6, Anchor::TOP_CENTER);

	std::wstring infoText = L"Select what to harvest:";
	ui.text = &ui.infoPanel->addElement<Text>(1, 1, infoText, Anchor::TOP_CENTER);



    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

PlantUI getPlantFrame() {
    PlantUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.infoPanel = &frame->addElement<Panel>(0, 10, 25, 6, Anchor::TOP_LEFT);
    std::wstring infoText = L"Select what to plant:";
    ui.text = &ui.infoPanel->addElement<Text>(1, 1, infoText, Anchor::TOP_LEFT);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

void PlantUI::configurePlantFrame() {
    for (auto& i : seeds) {
        Game::getInstance().getUIManager().getFrame(UI::Plant)->removeElement(i);
    }
    seeds.clear();

	std::unordered_map<std::string, int> result;
    auto& stockpiles = mainWorld.getStockpiles();
    for (auto& s : stockpiles) {
        const auto& list = s.getItems();
        for (auto& [loc, f] : list) {
            for (auto& item : f) {
                if (auto s = mainWorld.registry.try_get<Seed>(item)) {
                    if (!mainWorld.registry.get<Claimable>(item).claimed) {
                        auto& name = mainWorld.registry.get<Name>(item);
                        result[name.name]++;
                    }
                }
            }
        }
    }

    int offset = 0;

    if (result.empty()) {
        text->changeText(L"No seeds to plant");
    }
    else {
        text->changeText(L"Select what to plant:");
	}

    for (auto& [name, count] : result) {
        std::string plantStr = name + " x" + std::to_string(count);
        auto frame = Game::getInstance().getUIManager().getFrame(UI::Plant);
        auto button = &createButton(*frame,
            infoPanel->getXPos() + 1, 2 + infoPanel->getYPos() + offset * 3,
            std::wstring(plantStr.begin(), plantStr.end()), Anchor::TOP_LEFT);

        button->setClickFunction([name] {
			setMode(Mode::PLANT);
			Game::getInstance().selectedPlantItem = name;
            });

        GameState& gameState = Game::getInstance().gameState;

        int xFrustum = gameState.cameraState.xFrustum;
        int yFrustum = gameState.cameraState.yFrustum;

		button->setAnchorPosition(xFrustum, yFrustum);

        seeds.push_back(button);
        offset++;
    }
}

InfoUI getInfoFrame() {
    InfoUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.infoPanel = &frame->addElement<Panel>(0, -20, 20, 6, Anchor::BOTTOM_LEFT);
    ui.itemName= &frame->addElement<Text>(1, -24, L"", Anchor::BOTTOM_LEFT);
	ui.ingredients = &frame->addElement<Text>(1, -23, L"", Anchor::BOTTOM_LEFT);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}


void InfoUI::configureInfoFrame(std::string obj) {

    auto staticObject = ObjectRegistry::getInstance().getStaticObject(obj);
    auto& name = ObjectRegistry::getInstance().getStaticRegistry().get<Name>(staticObject).name;

    GameState& gameState = Game::getInstance().gameState;

    int xFrustum = gameState.cameraState.xFrustum;
    int yFrustum = gameState.cameraState.yFrustum;

    std::wstring wname = std::wstring(name.begin(), name.end());
    itemName->changeText(wname);
    itemName->setAnchorPosition(xFrustum, yFrustum);

    std::wstring ingredientStr = L"Ingredients:|";
    
    Craftable* c = ObjectRegistry::getInstance().getStaticRegistry().try_get<Craftable>(staticObject);
    if (c) {
        for (auto const& j : c->ingredients) {
            std::wstring ingredient = std::wstring(j.first.begin(), j.first.end()) + L": x" + std::to_wstring(j.second);
            ingredientStr += ingredient + L"|";
        }
    }

    ingredients->changeText(ingredientStr);
    ingredients->setAnchorPosition(xFrustum, yFrustum);
}

// Helpers from here down
Button& createButton(Frame& frame, int x, int y, const std::wstring& label, Anchor anchor) {
    std::wstring padded = L"" + label + L"";
    size_t width = padded.size();

    std::wstring horizontal(width, L'─');
    std::wstring horizontalBold(width, L'═');

    auto normal = std::vector<std::wstring>{
        L"┌" + horizontal + L"┐",
        L"│" + padded + L"│",
        L"└" + horizontal + L"┘"
    };

    auto hover = std::vector<std::wstring>{
        L"╔" + horizontalBold + L"╗",
        L"║" + padded + L"║",
        L"╚" + horizontalBold + L"╝"
    };

    return frame.addElement<Button>(x, y, normal, hover, hover, anchor);
}

void setMode(Mode mode) {

    GameState& gameState = Game::getInstance().gameState;

    Mode& currentMode = gameState.placingState.currentMode;

    if (currentMode == mode) {
        currentMode = Mode::NONE;
    }
    else {
        currentMode = mode;
    }
    gameState.placingState.placing = false;
}