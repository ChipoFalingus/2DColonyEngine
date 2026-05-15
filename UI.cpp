#include "UI.h"
#include "World.h"
#include "Game.h"

#include <thread>


// There's probably a better way to format this but its miles better than the nested stuff i had before

std::unordered_map<std::string, int> getFurnitureList();

// First menu when opening the game

MainMenuUI getMainMenuFrame() {
    MainMenuUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.new_game = &createButton(*frame, 0, 0, L"             Start            ", Anchor::CENTER);

    // Swaps to the InGameFrame and does some setup to make sure the world is ready
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

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

SettingsUI getSettingsFrame() {
    SettingsUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.text = &frame->addElement<Text>(50, 0, L"Settings Menu", Anchor::TOP_CENTER);
	ui.back = &createButton(*frame, 0, 0, L" < Back ", Anchor::TOP_CENTER);

    ui.back->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame(UI::Settings, UI::Main);
		});

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

WorldSettingsUI getWorldSettingsFrame() {
    WorldSettingsUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.text = &frame->addElement<Text>(50, 1, L"World Settings Menu", Anchor::TOP_CENTER);

    ui.back = &createButton(*frame, 0, 0, L" < Back ", Anchor::TOP_CENTER);

    ui.back->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.swapFrame(UI::WorldSettings, UI::Main);
        });

    ui.panel = &frame->addElement<Panel>(0, 3, xFrustum, yFrustum - 3, Anchor::TOP_CENTER);

    ui.waterLevel = &frame->addElement<Slider>(3, 6, -50, 50, 0, 20, false, Anchor::TOP_CENTER);

    ui.begin = &createButton(*frame, xFrustum - 11, yFrustum - 4, L" Begin! ", Anchor::TOP_CENTER);

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
    ui.panel = &frame->addElement<Panel>(0, 0, xFrustum, yFrustum, Anchor::TOP_CENTER);

    ui.text = &frame->addElement<Text>(1, yFrustum / 2 - 3, L"Loading...", Anchor::TOP_CENTER);

    int numChunks = (calculateMapSize() * calculateMapSize()) / (chunkDim * chunkDim);
    ui.chunks = &frame->addElement<Text>(1, yFrustum / 2 - 2, L"0/" + std::to_wstring(numChunks), Anchor::TOP_CENTER);

    ui.animation = &frame->addElement<Text>(1, yFrustum / 2, L"", Anchor::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Menu you see when you're in the game
InGameUI getInGameFrame() {
    InGameUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    Panel& infoPanel = frame->addElement<Panel>(0, 0, 30, 6, Anchor::TOP_CENTER);
    Panel& tileInfoPanel = frame->addElement<Panel>(30, 0, 30, 5, Anchor::TOP_CENTER);

    ui.playerPos = &infoPanel.addElement<Text>(1, 1, L"", Anchor::TOP_CENTER);
    ui.FPS = &infoPanel.addElement<Text>(1, 2, L"", Anchor::TOP_CENTER);
    ui.population = &infoPanel.addElement<Text>(1, 3, L"", Anchor::TOP_CENTER);
    ui.waterLevel = &infoPanel.addElement<Text>(1, 4, L"", Anchor::TOP_CENTER);

    ui.tileType = &tileInfoPanel.addElement<Text>(1, 1, L"", Anchor::TOP_CENTER);
    ui.tileItems = &tileInfoPanel.addElement<Text>(1, 2, L"", Anchor::TOP_CENTER);

    ui.placingDims = &frame->addElement<Text>(0, 0, L"", Anchor::TOP_CENTER);
    ui.villagerName = &frame->addElement<Text>(0, 0, L"", Anchor::TOP_CENTER);


    ui.buildButton = &createButton(*frame, 0, 46, L"     ─█ Build     ", Anchor::TOP_CENTER);

    ui.buildButton->setClickFunction([]() {
        setMode(Mode::NONE);
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Production);
        uiManager.addOrRemoveFrame(UI::Build);
        });

    ui.harvestButton = &createButton(*frame, 20, 46, L"    /♣ Harvest    ", Anchor::TOP_CENTER);

    ui.harvestButton->setClickFunction([]() {
        mainWorld.placementMode = PlacementMode::SQUARE;
        setMode(Mode::HARVEST);
        });

    ui.plantButton = &createButton(*frame, 40, 46, L"     W. Plant     ", Anchor::TOP_CENTER);

    ui.plantButton->setClickFunction([]() {
        setMode(Mode::PLANT);
        });

    ui.stockpileButton = &createButton(*frame, 60, 46, L"   == Stockpile   ", Anchor::TOP_CENTER);

    ui.stockpileButton->setClickFunction([]() {
        mainWorld.placementMode = PlacementMode::SQUARE;
        setMode(Mode::STOCKPILE);
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

MiniMapUI getMiniMapFrame() {
    MiniMapUI ui;
    auto frame = std::make_unique<Frame>();

    frame->setType(ui.type);
    ui.text = &frame->addElement<Text>(0, 0, L"Mini Map Mode", Anchor::TOP_CENTER);
    ui.seed = &frame->addElement<Text>(0, 1, L"Seed: " + std::to_wstring(seed), Anchor::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Pop-up menu that shows up when you select "Build"
BuildUI getBuildFrame() {
    BuildUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.structureButton = &createButton(*frame, 0, 31, L" Structure ", Anchor::TOP_CENTER);

    ui.structureButton->setClickFunction([&]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Structure);

        });


    ui.furnitureButton = &createButton(*frame, 0, 34, L" Furniture ", Anchor::TOP_CENTER);

    ui.furnitureButton->setClickFunction([&]() {

        auto& furnitureFrame = Game::getInstance().getFurnitureUI();
        furnitureFrame.configureFurnitureFrame();

        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Furniture);
		});

    ui.productionButton = &createButton(*frame, 0, 37, L"Production ", Anchor::TOP_CENTER);

    ui.productionButton->setClickFunction([&]() {
        mainWorld.placementMode = PlacementMode::SINGLE;
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Production);

        });

    ui.securityButton = &createButton(*frame, 0, 40, L" Security  ", Anchor::TOP_CENTER);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

ProductionUI getProductionFrame() {
    ProductionUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.carpentry_bench = &createButton(*frame, 15, 31, L" Carpentry Bench ", Anchor::TOP_CENTER);
	ui.stone_cutter = &createButton(*frame, 15, 34, L" Stone Cutter ", Anchor::TOP_CENTER);
	ui.furnace = &createButton(*frame, 15, 37, L" Furnace ", Anchor::TOP_CENTER);
    ui.anvil = &createButton(*frame, 15, 40, L" Anvil ", Anchor::TOP_CENTER);

    ui.carpentry_bench->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Carpentry Bench");
        });

    ui.carpentry_bench->setHoverFunction([&]() {
        configureInfo("Carpentry Bench");
		Game::getInstance().getUIManager().push(UI::Info);
		});

    ui.stone_cutter->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Stone Cutter");
        });

    ui.stone_cutter->setHoverFunction([&]() {
        configureInfo("Stone Cutter");
        Game::getInstance().getUIManager().push(UI::Info);
        });

    ui.furnace->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Furnace");
        });

    ui.furnace->setHoverFunction([&]() {
        configureInfo("Furnace");
        Game::getInstance().getUIManager().push(UI::Info);
        });

    ui.anvil->setClickFunction([&]() {
        setMode(Mode::BUILD);
        Game::getInstance().setBuildItem("Anvil");
        });

    ui.anvil->setHoverFunction([&]() {
        configureInfo("Anvil");
        Game::getInstance().getUIManager().push(UI::Info);
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Pop-up menu that shows up when you select "Structure" in the Build menu
StructureUI getStructureFrame() {
    StructureUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);


    ui.wood_wall = &createButton(*frame, 16, 31, L" Wooden Wall ", Anchor::TOP_CENTER);
    ui.stone_wall = &createButton(*frame, 16, 34, L" Stone Wall ", Anchor::TOP_CENTER);
    ui.wood_fence = &createButton(*frame, 16, 37, L" Wooden Floor ", Anchor::TOP_CENTER);
    ui.stone_fence = &createButton(*frame, 16, 40, L" Stone Floor ", Anchor::TOP_CENTER);
    ui.wood_floor = &createButton(*frame, 16, 43, L" ? ", Anchor::TOP_CENTER);

    ui.wood_wall->setClickFunction([&]() {
        setMode(Mode::BUILD);
        mainWorld.placementMode = PlacementMode::SQUARE;
        Game::getInstance().setBuildItem("Wooden Wall");
        });
    ui.stone_wall->setClickFunction([&]() {
        setMode(Mode::BUILD);
        mainWorld.placementMode = PlacementMode::SQUARE;
        Game::getInstance().setBuildItem("Stone Wall");
        });
    ui.wood_fence->setClickFunction([&]() {
        setMode(Mode::BUILD);
        mainWorld.placementMode = PlacementMode::SQUARE;
        Game::getInstance().setBuildItem("Wooden Floor");
        });
    ui.stone_fence->setClickFunction([&]() {
        setMode(Mode::BUILD);
        mainWorld.placementMode = PlacementMode::SQUARE;
        Game::getInstance().setBuildItem("Stone Floor");
        });

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}


FurnitureUI getFurnitureFrame() {
    FurnitureUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    int offset = 0;
    for (auto& [f, num] : getFurnitureList()) {

        std::string j = f + " x" + std::to_string(num);

        auto button = &createButton(*frame,
            16, 49 + offset * 3,
            std::wstring(j.begin(), j.end()), Anchor::TOP_CENTER);

        ui.buttons.push_back(button);
        offset++;
    }

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

void FurnitureUI::configureFurnitureFrame() {
    buttons.clear();

    auto i = getFurnitureList();
    int offset = 0;
    for (auto& [string, num] : i) {
        std::string j = string + " x" + std::to_string(num);
        auto frame = Game::getInstance().getUIManager().getFrame(UI::Furniture);
        auto button = &createButton(*frame,
            16, 40 + offset * 3,
            std::wstring(j.begin(), j.end()), Anchor::TOP_CENTER);

        button->setClickFunction([this, string]() {
            setMode(Mode::BUILD);
            mainWorld.placementMode = PlacementMode::SINGLE;
            std::cout << string << std::endl;
            Game::getInstance().setBuildItem(string);
            Game::getInstance().getUIManager().remove(UI::Furniture);
            placing = true;
            });

        buttons.push_back(button);
        offset--;
    }
}

std::unordered_map<std::string, int> getFurnitureList() {

    std::unordered_map<std::string, int> result;

    auto& stockpiles = mainWorld.getStockpiles();
    for (auto& s : stockpiles) {
        const auto& list = s.getItems();
        for (auto& [loc, f] : list) {
            for (auto& item : f) {
                if (item->type == Type::Furniture && !item->claimed) {
                    result[item->name]++;
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

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_CENTER);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_CENTER);
	ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_CENTER);

    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Carpentry Bench") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.second.result.begin(), i.second.result.end()), Anchor::TOP_CENTER);

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

	ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_CENTER);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Carpentry);
		});

    ui.panel->setSize(35, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

AnvilUI getAnvilFrame() {
    AnvilUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_CENTER);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_CENTER);

    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Anvil") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.first.begin(), i.first.end()), Anchor::TOP_CENTER);

        std::string recipeName = i.first;

        button->setClickFunction([recipeName] {
            JobManager::addJob(new Craft(nullptr, nullptr, JobType::Blacksmith, recipeName));
            //Game::getInstance().setBuildItem(recipeName);
            std::cout << "Selected: " << recipeName << std::endl;
            });

        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 30, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_CENTER);

    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Anvil);
        });

    ui.panel->setSize(34, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

GunBenchUI getGunBenchFrame() {
    GunBenchUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_CENTER);
    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_CENTER);
    ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_CENTER);
    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Gun Bench") continue;
        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.second.result.begin(), i.second.result.end()), Anchor::TOP_CENTER);
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

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 30, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_CENTER);

    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Gun);
        });

    ui.panel->setSize(34, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

VillagerInfoUI getVillagerInfoFrame() {
    VillagerInfoUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.infoPanel = &frame->addElement<Panel>(0, 10, 6, 7, Anchor::TOP_CENTER);

    ui.text = &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_CENTER);
	ui.job = &ui.infoPanel->addElement<Text>(1, 2, L"", Anchor::TOP_CENTER);
    ui.inventory = &ui.infoPanel->addElement<Text>(1, 3, L"", Anchor::TOP_CENTER);

    ui.health = &ui.infoPanel->addElement<Text>(1, 4, L"", Anchor::TOP_CENTER);
    ui.hunger = &ui.infoPanel->addElement<Text>(1, 5, L"", Anchor::TOP_CENTER);

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

InfoUI getInfoFrame() {
    InfoUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.infoPanel = &frame->addElement<Panel>(0, 25, 20, 6, Anchor::TOP_CENTER);
    ui.itemName= &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_CENTER);
	ui.ingredients = &ui.infoPanel->addElement<Text>(1, 2, L"", Anchor::TOP_CENTER);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

// Helpers from here down
Button& createButton(
    Frame& frame,
    int x, int y,
    const std::wstring& label,
    Anchor anchor
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

    return frame.addElement<Button>(x, y, normal, hover, hover, anchor);
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