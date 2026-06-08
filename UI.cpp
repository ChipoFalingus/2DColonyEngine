#include "UI.h"
#include "World.h"
#include "Game.h"
#include "JobType.h"
#include "Seed.h"

#include <thread>


// There's probably a better way to format this but its miles better than the nested stuff i had before

std::unordered_map<std::string, int> getFurnitureList();

// First menu when opening the game

MainMenuUI getMainMenuFrame() {
    MainMenuUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

	ui.title = &frame->addElement<Text>(0, -10, L"Fling's Colony", Anchor::CENTER);

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

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
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

    ui.panel = &frame->addElement<Panel>(0, 3, xFrustum, yFrustum - 3, Anchor::TOP_CENTER);

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

    Panel& infoPanel = frame->addElement<Panel>(0, 0, 30, 6, Anchor::TOP_CENTER);
    Panel& tileInfoPanel = frame->addElement<Panel>(30, 0, 30, 5, Anchor::TOP_CENTER);

    ui.playerPos = &infoPanel.addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
    ui.FPS = &infoPanel.addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);
    ui.population = &infoPanel.addElement<Text>(1, 3, L"", Anchor::TOP_LEFT);
    ui.waterLevel = &infoPanel.addElement<Text>(1, 4, L"", Anchor::TOP_LEFT);

    ui.tileType = &tileInfoPanel.addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
    ui.tileItems = &tileInfoPanel.addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);

    ui.placingDims = &frame->addElement<Text>(0, 0, L"", Anchor::TOP_LEFT);
    ui.villagerName = &frame->addElement<Text>(0, 0, L"", Anchor::TOP_CENTER);

	ui.day = &frame->addElement<Text>(0, 0, L"", Anchor::BOTTOM_RIGHT);

    ui.buildButton = &createButton(*frame, 0, 0, L"     ─█ Build     ", Anchor::BOTTOM_LEFT);

    ui.buildButton->setClickFunction([]() {
        setMode(Mode::NONE);
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Production);
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
        mainWorld.placementMode = PlacementMode::SQUARE;
        setMode(Mode::STOCKPILE);
        });

	ui.villagers = &createButton(*frame, 0, 0, L"☻", Anchor::TOP_RIGHT);

    ui.villagers->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
		Game::getInstance().getVillagerListUI().configureVillagerList();
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
    ui.text = &frame->addElement<Text>(0, 0, L"Villagers", Anchor::TOP_LEFT);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

void VillagerListUI::configureVillagerList() {
    for (auto& button : villagers) {
        Game::getInstance().getUIManager().getFrame(UI::VillagerList)->removeElement(button);
    }

    villagers.clear();

	auto frame = Game::getInstance().getUIManager().getFrame(UI::VillagerList);

    int i = 0;
    for (auto& villager : mainWorld.getAllVillagers()) {
		auto panel = &frame->addElement<Panel>(0, i*5, 30, 5, Anchor::CENTER_LEFT);
		std::string name = villager->firstname + " " + villager->lastname;
		panel->addElement<Text>(1, 0, std::wstring(name.begin(), name.end()), Anchor::TOP_LEFT);

		std::string position = "Position: (" + std::to_string(villager->xPos) + ", " + std::to_string(villager->yPos) + ")";
        panel->addElement<Text>(4, 1, std::wstring(position.begin(), position.end()), Anchor::TOP_LEFT);

		std::string status = "Status: " + activityStateToString(villager->activity_state);
        panel->addElement<Text>(4, 2, std::wstring(status.begin(), status.end()), Anchor::TOP_LEFT);

		panel->addElement<Text>(2, 2, L"☺", Anchor::TOP_LEFT);

        villagers.push_back(panel);

        panel->setAnchorPosition(xFrustum, yFrustum);
        i++;
	}
}

// Pop-up menu that shows up when you select "Build"
BuildUI getBuildFrame() {
    BuildUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.structureButton = &createButton(*frame, 0, -14, L" Structure ", Anchor::BOTTOM_LEFT);

    ui.structureButton->setClickFunction([&]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Structure);

        uiManager.remove(UI::Furniture);
        uiManager.remove(UI::Production);
        });


    ui.furnitureButton = &createButton(*frame, 0, -11, L" Furniture ", Anchor::BOTTOM_LEFT);

    ui.furnitureButton->setClickFunction([&]() {

        auto& furnitureFrame = Game::getInstance().getFurnitureUI();
        furnitureFrame.configureFurnitureFrame();

        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Furniture);

        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Production);
		});

    ui.productionButton = &createButton(*frame, 0, -8, L"Production ", Anchor::BOTTOM_LEFT);

    ui.productionButton->setClickFunction([&]() {
        mainWorld.placementMode = PlacementMode::SINGLE;
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.addOrRemoveFrame(UI::Production);

        uiManager.remove(UI::Structure);
        uiManager.remove(UI::Furniture);
        });

    ui.securityButton = &createButton(*frame, 0, -5, L" Security  ", Anchor::BOTTOM_LEFT);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

ProductionUI getProductionFrame() {
    ProductionUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.carpentry_bench = &createButton(*frame, 14, -14, L" Carpentry Bench ", Anchor::BOTTOM_LEFT);
	ui.stone_cutter = &createButton(*frame, 14, -11, L" Stone Cutter ", Anchor::BOTTOM_LEFT);
	ui.furnace = &createButton(*frame, 14, -8, L" Furnace ", Anchor::BOTTOM_LEFT);
    ui.anvil = &createButton(*frame, 14, -5, L" Anvil ", Anchor::BOTTOM_LEFT);

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


    ui.wood_wall = &createButton(*frame, 14, -17, L" Wooden Wall ", Anchor::BOTTOM_LEFT);
    ui.stone_wall = &createButton(*frame, 14, -14, L" Stone Wall ", Anchor::BOTTOM_LEFT);
    ui.wood_fence = &createButton(*frame, 14, -11, L" Wooden Floor ", Anchor::BOTTOM_LEFT);
    ui.stone_fence = &createButton(*frame, 14, -8, L" Stone Floor ", Anchor::BOTTOM_LEFT);
    ui.wood_floor = &createButton(*frame, 14, -5, L" ? ", Anchor::BOTTOM_LEFT);
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
            placing = true;
            });

        button->setAnchorPosition(xFrustum, yFrustum);
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

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);
	ui.ingredients = &ui.panel->addElement<Text>(17, 5, L"", Anchor::TOP_LEFT);

    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {

        if (i.second.requiredStation != "Carpentry Bench") continue;

        std::cout << "Adding recipe button for " << i.first << std::endl;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.second.result.begin(), i.second.result.end()), Anchor::TOP_LEFT);

        std::string recipeName = i.first;

        button->setClickFunction([recipeName] {
            Job* job = new Craft(nullptr, nullptr, SkillType::Carpentry, recipeName);
            job->priority = 25;
            JobManager::addJob(job);
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

		button->setAnchorPosition(xFrustum, yFrustum);

        ui.craftable_items.push_back(button);
        num++;
    }

	ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);


    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::Carpentry);
		});

    ui.panel->setSize(35, num * 3 + 3);

    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

StoneCutterUI getStoneCutterFrame() {
    StoneCutterUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_CENTER);
    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);
    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Stone Cutter") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.second.result.begin(), i.second.result.end()), Anchor::TOP_LEFT);

        std::string recipeName = i.first;

        button->setClickFunction([recipeName] {
            JobManager::addJob(new Craft(nullptr, nullptr, SkillType::Masonry, recipeName));
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

        button->setAnchorPosition(xFrustum, yFrustum);

        ui.craftable_items.push_back(button);
        num++;
    }
    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 31, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);
    ui.closeButton->setClickFunction([]() {
        auto& uiManager = Game::getInstance().getUIManager();
        uiManager.remove(UI::StoneCutter);
        });
    ui.panel->setSize(35, num * 3 + 3);
    Game::getInstance().getUIManager().addFrame(std::move(frame), ui.type);
    return ui;
}

AnvilUI getAnvilFrame() {
    AnvilUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.panel = &frame->addElement<Panel>(10, 10, 30, 5, Anchor::TOP_LEFT);

    ui.text = &ui.panel->addElement<Text>(1, 1, L"Select something to craft:", Anchor::TOP_LEFT);

    int num = 0;
    for (auto& i : RecipeRegistry::getInstance().getRecipeTable()) {
        if (i.second.requiredStation != "Anvil") continue;

        auto button = &createButton(*frame,
            ui.panel->getXOffset() + 1, ui.panel->getXOffset() + num * 3 + 2,
            std::wstring(i.first.begin(), i.first.end()), Anchor::TOP_LEFT);

        std::string recipeName = i.first;

        button->setClickFunction([recipeName] {
            JobManager::addJob(new Craft(nullptr, nullptr, SkillType::Blacksmithing, recipeName));
            //Game::getInstance().setBuildItem(recipeName);
            std::cout << "Selected: " << recipeName << std::endl;
            });

        ui.craftable_items.push_back(button);
        num++;
    }

    ui.closeButton = &createButton(*frame, ui.panel->getXOffset() + 30, ui.panel->getXOffset() + 1, L"X", Anchor::TOP_LEFT);

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
            JobManager::addJob(new Craft(nullptr, nullptr, SkillType::Gunsmithing, recipeName));
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

StockpileUI getStockpileFrame() {
    StockpileUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.infoPanel = &frame->addElement<Panel>(0, 14, 6, 7, Anchor::TOP_LEFT );
    ui.text = &ui.infoPanel->addElement<Text>(20, 1, L"Filters:", Anchor::TOP_LEFT);
    ui.capacity = &ui.infoPanel->addElement<Text>(40, 2, L"", Anchor::TOP_LEFT);
    ui.contents = &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);

    int num = ui.infoPanel->getYOffset() + 2;
    for (int i = 0; i < getAllTypes().size(); i++) {
		std::string type = itemTypeToString(getAllTypes()[i]);
		std::wstring typeName = std::wstring(type.begin(), type.end());
		
		ui.typeFilters.push_back(&frame->addElement<Checkbox>(20, num, Anchor::TOP_LEFT));

		frame->addElement<Text>(23, num, typeName, Anchor::TOP_LEFT);
        num++;
	}

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

    int num = infoPanel->getYOffset() + 2;
    for (int i = 0; i < getAllTypes().size(); i++) {
        std::string type = itemTypeToString(getAllTypes()[i]);
        std::wstring typeName = std::wstring(type.begin(), type.end());

		auto frame = Game::getInstance().getUIManager().getFrame(UI::Stockpile);
		auto checkbox = &frame->addElement<Checkbox>(20, num, Anchor::TOP_CENTER);
		//checkbox->setChecked(stockpile.getFilter().at(getAllTypes()[i]));
        typeFilters.push_back(checkbox);

        //frame->addElement<Text>(23, num, typeName, Anchor::TOP_CENTER);
        num++;
    }

	infoPanel->setSize(40, 14);

    contents->changeText(contentsStr);
}

VillagerInfoUI getVillagerInfoFrame() {
    VillagerInfoUI ui;

    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);

    ui.infoPanel = &frame->addElement<Panel>(0, 10, 6, 17, Anchor::TOP_LEFT);

    ui.text = &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
	ui.job = &ui.infoPanel->addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);
    ui.inventory = &ui.infoPanel->addElement<Text>(1, 3, L"", Anchor::TOP_LEFT);
    ui.health = &ui.infoPanel->addElement<Text>(1, 4, L"", Anchor::TOP_LEFT);
    ui.hunger = &ui.infoPanel->addElement<Text>(1, 5, L"", Anchor::TOP_LEFT);

	ui.skills = &ui.infoPanel->addElement<Text>(1, 6, L"", Anchor::TOP_LEFT);

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
                if (item->type == Type::Seed && !item->claimed) {
                    result[item->name]++;
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

		button->setAnchorPosition(xFrustum, yFrustum);

        seeds.push_back(button);
        offset++;
    }
}

InfoUI getInfoFrame() {
    InfoUI ui;
    auto frame = std::make_unique<Frame>();
    frame->setType(ui.type);
    ui.infoPanel = &frame->addElement<Panel>(0, 25, 20, 6, Anchor::BOTTOM_LEFT);
    ui.itemName= &ui.infoPanel->addElement<Text>(1, 1, L"", Anchor::TOP_LEFT);
	ui.ingredients = &ui.infoPanel->addElement<Text>(1, 2, L"", Anchor::TOP_LEFT);
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