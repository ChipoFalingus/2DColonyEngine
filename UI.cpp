#include "UI.h"
#include "World.h"
#include "Game.h"

#include <thread>


// There's probably a better way to format this but its miles better than the nested stuff i had before

// First menu when opening the game

MainMenuUI getMainMenuFrame() {
    MainMenuUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.new_game = &createButton(*frame, 0, 0, L"             Start            ", Alignment::TOP_CENTER);

    // Swaps to the InGameFrame and does some setup to make sure the world is ready
    ui.new_game->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame("main", "worldsettings");

        //mainWorld.getChunks().clear();
        //mainWorld.generateWorld();
        });

    ui.load = &createButton(*frame, 0, 3, L"           Load Game          ", Alignment::TOP_CENTER);
    ui.settingsButton = &createButton(*frame, 0, 6, L"           Settings           ", Alignment::TOP_CENTER);

    ui.settingsButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
		uiManager.swapFrame("main", "settings");
		});

    ui.exit = &createButton(*frame, 0, 9, L"             Exit             ", Alignment::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

SettingsUI getSettingsFrame() {
    SettingsUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);
    ui.text = &frame->addElement<Text>(50, 0, L"Settings Menu", Alignment::TOP_CENTER);
	ui.back = &createButton(*frame, 0, 0, L" < Back ", Alignment::TOP_CENTER);

    ui.back->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame("settings", "main");
		});

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

WorldSettingsUI getWorldSettingsFrame() {
    WorldSettingsUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.text = &frame->addElement<Text>(50, 1, L"World Settings Menu", Alignment::TOP_CENTER);

    ui.back = &createButton(*frame, 0, 0, L" < Back ", Alignment::TOP_CENTER);

    ui.back->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame("worldsettings", "main");
        });

    ui.panel = &frame->addElement<Panel>(0, 3, xFrustum, yFrustum - 3, Alignment::TOP_CENTER);

    ui.begin = &createButton(*frame, xFrustum - 11, yFrustum - 4, L" Begin! ", Alignment::TOP_CENTER);

    ui.begin->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame("worldsettings", "loading");

        mainWorld.getChunks().clear();
        mainWorld.generateWorld();

        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

LoadingUI getLoadingFrame() {
    LoadingUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);
    ui.panel = &frame->addElement<Panel>(0, 0, xFrustum, yFrustum, Alignment::TOP_CENTER);

    ui.text = &frame->addElement<Text>(1, yFrustum / 2 - 3, L"Loading...", Alignment::TOP_CENTER);

    int numChunks = (calculateMapSize() * calculateMapSize()) / (chunkDim * chunkDim);
    ui.chunks = &frame->addElement<Text>(1, yFrustum / 2 - 2, L"0/" + std::to_wstring(numChunks), Alignment::TOP_CENTER);

    ui.animation = &frame->addElement<Text>(1, yFrustum / 2, L"", Alignment::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

// Menu you see when you're in the game
InGameUI getInGameFrame() {
    InGameUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    Panel& infoPanel = frame->addElement<Panel>(0, 0, 30, 6, Alignment::TOP_CENTER);
    Panel& tileInfoPanel = frame->addElement<Panel>(30, 0, 30, 5, Alignment::TOP_CENTER);

    ui.playerPos = &infoPanel.addElement<Text>(1, 1, L"", Alignment::TOP_CENTER);
    ui.FPS = &infoPanel.addElement<Text>(1, 2, L"", Alignment::TOP_CENTER);
    ui.population = &infoPanel.addElement<Text>(1, 3, L"", Alignment::TOP_CENTER);
    ui.waterLevel = &infoPanel.addElement<Text>(1, 4, L"", Alignment::TOP_CENTER);

    ui.tileType = &tileInfoPanel.addElement<Text>(1, 1, L"", Alignment::TOP_CENTER);
    ui.tileItems = &tileInfoPanel.addElement<Text>(1, 2, L"", Alignment::TOP_CENTER);

    ui.placingDims = &frame->addElement<Text>(0, 0, L"", Alignment::TOP_CENTER);
    ui.villagerName = &frame->addElement<Text>(0, 0, L"", Alignment::TOP_CENTER);


    ui.buildButton = &createButton(*frame, 0, 46, L"     ─█ Build     ", Alignment::TOP_CENTER);

    ui.buildButton->setClickFunction([]() {
        setMode(Mode::NONE);
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove("structure");
        uiManager.remove("production");
        uiManager.addOrRemoveFrame("build");
        });

    ui.harvestButton = &createButton(*frame, 20, 46, L"    /♣ Harvest    ", Alignment::TOP_CENTER);

    ui.harvestButton->setClickFunction([]() {
        setMode(Mode::HARVEST);
        });

    ui.plantButton = &createButton(*frame, 40, 46, L"     W. Plant     ", Alignment::TOP_CENTER);

    ui.plantButton->setClickFunction([]() {
        setMode(Mode::PLANT);
        });

    ui.stockpileButton = &createButton(*frame, 60, 46, L"   == Stockpile   ", Alignment::TOP_CENTER);

    ui.stockpileButton->setClickFunction([]() {
        setMode(Mode::STOCKPILE);
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

MiniMapUI getMiniMapFrame() {
    MiniMapUI ui;
    auto frame = std::make_unique<Frame>();

    frame->setName(ui.name);
    ui.text = &frame->addElement<Text>(0, 0, L"Mini Map Mode", Alignment::TOP_CENTER);
    ui.seed = &frame->addElement<Text>(0, 1, L"Seed: " + std::to_wstring(seed), Alignment::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

// Pop-up menu that shows up when you select "Build"
BuildUI getBuildFrame() {
    BuildUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.structureButton = &createButton(*frame, 0, 31, L" Structure ", Alignment::TOP_CENTER);

    ui.structureButton->setClickFunction([&]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame("structure");

        });


    ui.furnitureButton = &createButton(*frame, 0, 34, L" Furniture ", Alignment::TOP_CENTER);

    ui.furnitureButton->setClickFunction([&]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame("furniture");
		});

    ui.productionButton = &createButton(*frame, 0, 37, L"Production ", Alignment::TOP_CENTER);

    ui.productionButton->setClickFunction([&]() {
        mainWorld.placementMode = PlacementMode::SINGLE;
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame("production");

        });

    ui.securityButton = &createButton(*frame, 0, 40, L" Security  ", Alignment::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

ProductionUI getProductionFrame() {
    ProductionUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.carpentry_bench = &createButton(*frame, 15, 31, L" Gun Bench ", Alignment::TOP_CENTER);
	ui.stone_cutter = &createButton(*frame, 15, 34, L" Stone Cutter ", Alignment::TOP_CENTER);
	ui.furnace = &createButton(*frame, 15, 37, L" Furnace ", Alignment::TOP_CENTER);
    ui.anvil = &createButton(*frame, 15, 40, L" Anvil ", Alignment::TOP_CENTER);

    ui.carpentry_bench->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Gun Bench");
        });

    ui.carpentry_bench->setHoverFunction([&]() {
        configureInfo("Gun Bench");
		Game::getInstance().getUIManager().push("info");
		});

    ui.stone_cutter->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Stone Cutter");
        });

    ui.stone_cutter->setHoverFunction([&]() {
        configureInfo("Stone Cutter");
        Game::getInstance().getUIManager().push("info");
        });

    ui.furnace->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Furnace");
        });

    ui.furnace->setHoverFunction([&]() {
        configureInfo("Furnace");
        Game::getInstance().getUIManager().push("info");
        });

    ui.anvil->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Anvil");
        });

    ui.anvil->setHoverFunction([&]() {
        configureInfo("Anvil");
        Game::getInstance().getUIManager().push("info");
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

// Pop-up menu that shows up when you select "Structure" in the Build menu
StructureUI getStructureFrame() {
    StructureUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);


    ui.wood_wall = &createButton(*frame, 15, 43, L" Wall ", Alignment::TOP_CENTER);
    ui.stone_wall = &createButton(*frame, ui.wood_wall->getXLength() + ui.wood_wall->getXOffset(), 43, L" Wall ", Alignment::TOP_CENTER);
    ui.wood_fence = &createButton(*frame, ui.stone_wall->getXLength() + ui.stone_wall->getXOffset(), 43, L" Chair ", Alignment::TOP_CENTER);
    ui.stone_fence = &createButton(*frame, ui.wood_fence->getXLength() + ui.wood_fence->getXOffset(), 43, L" Bed ", Alignment::TOP_CENTER);
    ui.wood_floor = &createButton(*frame, ui.stone_fence->getXLength() + ui.stone_fence->getXOffset(), 43, L" Floor ", Alignment::TOP_CENTER);

    ui.wood_wall->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Wooden Wall");
        });

    ui.stone_wall->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Stone Wall");
        });
    ui.wood_fence->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Chair");
        });
    ui.stone_fence->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Bed");
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

FurnitureUI getFurnitureFrame() {
    FurnitureUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);
    ui.chair = &createButton(*frame, 15, 43, L" Chair ", Alignment::TOP_CENTER);
    ui.bed = &createButton(*frame, ui.chair->getXLength() + ui.chair->getXOffset(), 43, L" Bed ", Alignment::TOP_CENTER);
    ui.chair->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Chair");
        });
    ui.bed->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Bed");
        });
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

CarpentryBenchUI getCarpentryBenchFrame() {
    CarpentryBenchUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Alignment::TOP_CENTER);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Alignment::TOP_CENTER);
	ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Alignment::TOP_CENTER);

    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Carpentry Bench") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.second.result.begin(), i.second.result.end()), Alignment::TOP_CENTER);

        std::string recipeName = i.first;

        button->setClickFunction([recipeName] {
            JobManager::addJob(new Craft(nullptr, nullptr, JobType::Carpenter, recipeName));
            });

        button->setHoverFunction([ui, recipeName] {
			std::string infoStr = "Ingredients:|";
			auto recipe = RecipeRegistry::getInstance().get(recipeName);
            for (auto& j : recipe->ingredients) {
                std::string ingredientStr = j.first + ": x" + std::to_string(j.second);
				infoStr += ingredientStr + "|";
				
            }
            infoStr += "|";
            infoStr += "Produces:|";
			infoStr +=  "x" + std::to_string(recipe->quantity) + " " + recipe->result;
			ui.ingredients->changeText(std::wstring(infoStr.begin(), infoStr.end()));
			});

        ui.craftable_items.push_back(button);
        num++;
    }

	ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Alignment::TOP_CENTER);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove("carpentry");
		});

    ui.panel->setSize(35, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

AnvilUI getAnvilFrame() {
    AnvilUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Alignment::TOP_CENTER);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Alignment::TOP_CENTER);

    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Anvil") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.first.begin(), i.first.end()), Alignment::TOP_CENTER);

        std::string recipeName = i.first;

        button->setClickFunction([recipeName] {
            JobManager::addJob(new Craft(nullptr, nullptr, JobType::Blacksmith, recipeName));
            //Game::getInstance().setBuildItem(recipeName);
            std::cout << "Selected: " << recipeName << std::endl;
            });

        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 30, ui.panel->getXOffset() + 1, L"X", Alignment::TOP_CENTER);

    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove("anvil");
        });

    ui.panel->setSize(34, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

GunBenchUI getGunBenchFrame() {
    GunBenchUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);
    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Alignment::TOP_CENTER);
    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Alignment::TOP_CENTER);
    ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Alignment::TOP_CENTER);
    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Gun Bench") continue;
        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.second.result.begin(), i.second.result.end()), Alignment::TOP_CENTER);
        std::string recipeName = i.first;


        button->setClickFunction([recipeName] {
            JobManager::addJob(new Craft(nullptr, nullptr, JobType::None, recipeName));
            });


        button->setHoverFunction([ui, recipeName] {
            std::string infoStr = "Ingredients:|";
            auto recipe = RecipeRegistry::getInstance().get(recipeName);
            for (auto& j : recipe->ingredients) {
                std::string ingredientStr = j.first + ": x" + std::to_string(j.second);
                infoStr += ingredientStr + "|";

            }
            infoStr += "|";
            infoStr += "Produces:|";
            infoStr += "x" + std::to_string(recipe->quantity) + " " + recipe->result;
            ui.ingredients->changeText(std::wstring(infoStr.begin(), infoStr.end()));
            });
        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 30, ui.panel->getXOffset() + 1, L"X", Alignment::TOP_CENTER);

    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove("gun");
        });

    ui.panel->setSize(34, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

VillagerInfoUI getVillagerInfoFrame() {
    VillagerInfoUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);

    ui.infoPanel = &frame->addElement<Panel>(0, 10, 6, 6, Alignment::TOP_CENTER);

    ui.text = &ui.infoPanel->addElement<Text>(1, 1, L"", Alignment::TOP_CENTER);
	ui.job = &ui.infoPanel->addElement<Text>(1, 2, L"", Alignment::TOP_CENTER);

    ui.health = &ui.infoPanel->addElement<Text>(1, 4, L"", Alignment::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

HarvestUI getHarvestFrame() {
    HarvestUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);
    
	ui.infoPanel = &frame->addElement<Panel>(0, 10, 6, 6, Alignment::TOP_CENTER);

	std::wstring infoText = L"Select what to harvest:";
	ui.text = &ui.infoPanel->addElement<Text>(1, 1, infoText, Alignment::TOP_CENTER);



    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

InfoUI getInfoFrame() {
    InfoUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setName(ui.name);
    ui.infoPanel = &frame->addElement<Panel>(0, 25, 20, 6, Alignment::TOP_CENTER);
    ui.itemName= &ui.infoPanel->addElement<Text>(1, 1, L"", Alignment::TOP_CENTER);
	ui.ingredients = &ui.infoPanel->addElement<Text>(1, 2, L"", Alignment::TOP_CENTER);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.name);
    return ui;
}

// Helpers from here down
Button& createButton(
    Frame& frame,
    int x, int y,
    const std::wstring& label,
    Alignment align
) {
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

    return frame.addElement<Button>(x, y, normal, hover, hover, align);
}


void configureInfo(std::string obj) {

    InfoUI& infoUI = Game::getInstance().getInfoUI();

	auto object = ObjectRegistry::getInstance().get(obj);

	std::wstring name = std::wstring(object->name.begin(), object->name.end());
    infoUI.itemName->changeText(name);

	std::wstring ingredientStr = L"Ingredients:|";
    auto recipe = RecipeRegistry::getInstance().get(obj);
    if (recipe) {
        for (auto& j : RecipeRegistry::getInstance().get(object->name)->ingredients) {
            std::wstring ingredient = std::wstring(j.first.begin(), j.first.end()) + L": x" + std::to_wstring(j.second);
            ingredientStr += ingredient + L"|";
        }
    }

    infoUI.ingredients->changeText(ingredientStr);
}

void setMode(Mode mode) {
    if (currentMode == mode) {
        currentMode = Mode::NONE;
    }
    else {
        currentMode = mode;
    }
    placing = false;
}