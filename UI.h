#pragma once
#include "UIElements.h"
#include "Stockpile.h"

struct MainMenuUI {
    UI type = UI::Main;

    Button* new_game;
    Button* load;
    Button* settingsButton;
    Button* exit;

    void layout(int screenW, int screenH) {
        new_game->setAnchorPosition(screenW, screenH);
        load->setAnchorPosition(screenW, screenH);
        settingsButton->setAnchorPosition(screenW, screenH);
        exit->setAnchorPosition(screenW, screenH);

        std::cout << "New Game Button at " << 
            new_game->getAnchorPosition(screenW, screenH).first << " " << 
            new_game->getAnchorPosition(screenW, screenH).second << std::endl;
    }
};

MainMenuUI getMainMenuFrame();

struct SettingsUI {
    UI type = UI::Settings;

	Text* text;
    Button* back;
};

SettingsUI getSettingsFrame();

struct WorldSettingsUI {
    UI type = UI::WorldSettings;

    Text* text;
    Button* back;

    Panel* panel;
    Slider* waterLevel;

    Button* begin;
};

WorldSettingsUI getWorldSettingsFrame();

struct LoadingUI {
    UI type = UI::Loading;

    Panel* panel;
    Text* text;
    Text* chunks;
	Text* animation;
};

LoadingUI getLoadingFrame();

struct InGameUI {
    UI type = UI::InGame;

    Text* playerPos;
    Text* FPS;
    Text* population;
    Text* waterLevel;

    Text* tileType;
    Text* tileItems;

    Text* placingDims;
    Text* villagerName;

    Button* buildButton;
    Button* harvestButton;
    Button* plantButton;
    Button* stockpileButton;
};

InGameUI getInGameFrame();

struct MiniMapUI {
    UI type = UI::Minimap;
    Text* text;
    Text* seed;
};

MiniMapUI getMiniMapFrame();

struct BuildUI {
    UI type = UI::Build;

    Button* structureButton;
    Button* furnitureButton;
    Button* productionButton;
    Button* securityButton;
};

BuildUI getBuildFrame();

struct ProductionUI {
    UI type = UI::Production;

    Button* carpentry_bench;
	Button* stone_cutter;
	Button* furnace;
    Button* anvil;
};

ProductionUI getProductionFrame();

struct StructureUI {
    UI type = UI::Structure;

    Button* wood_wall;
    Button* stone_wall;
    Button* wood_fence;
    Button* stone_fence;
    Button* wood_floor;
};

StructureUI getStructureFrame();

struct FurnitureUI {
    UI type = UI::Furniture;

    Panel* panel;
    std::vector<Button*> buttons;

    void configureFurnitureFrame();
};

FurnitureUI getFurnitureFrame();

struct CarpentryBenchUI {
    UI type = UI::Carpentry;

    Text* text;
    Text* ingredients;

    Panel* panel;
    std::vector<Button*> craftable_items;

	Button* closeButton;
};

CarpentryBenchUI getCarpentryBenchFrame();

struct AnvilUI {
    UI type = UI::Anvil;
    Text* text;
    Panel* panel;
    std::vector<Button*> craftable_items;
    Button* closeButton;
};

AnvilUI getAnvilFrame();

struct GunBenchUI {
    UI type = UI::Gun;
    Text* text;
    Text* ingredients;
    Panel* panel;
    std::vector<Button*> craftable_items;
    Button* closeButton;
};

GunBenchUI getGunBenchFrame();

struct StockpileUI {
    UI type = UI::Stockpile;
    Panel* infoPanel;
    Text* text;
    Text* capacity;
    Text* contents;

	std::vector<Checkbox*> typeFilters;

    void updateStockpileUI(Stockpile& stockpile);
};

StockpileUI getStockpileFrame();

struct VillagerInfoUI {
    UI type = UI::Villager;

    Panel* infoPanel;

    Text* text;
    Text* job;
    Text* inventory;

    Text* hunger;
    Text* health;

    Text* skills;
};

VillagerInfoUI getVillagerInfoFrame();

struct HarvestUI {
    UI type = UI::Harvest;

	Panel* infoPanel;

    Text* text;

	std::vector<Text*> harvestable_item_texts;
    std::vector<Checkbox*> harvestable_items;
};

HarvestUI getHarvestFrame();

struct InfoUI {
    UI type = UI::Info;
    Panel* infoPanel;
	Text* itemName;
    Text* ingredients;
	Text* description;
};

InfoUI getInfoFrame();

Button& createButton(
    Frame& frame,
    int x, int y,
    const std::wstring& label,
    Anchor anchor
);

void setMode(Mode mode);
void configureInfo(std::string obj);