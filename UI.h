#pragma once
#include "UIElements.h"

struct MainMenuUI {
    std::string name = "main";

    Button* new_game;
    Button* load;
    Button* settingsButton;
    Button* exit;
};

MainMenuUI getMainMenuFrame();

struct SettingsUI {
    std::string name = "settings";

	Text* text;
    Button* back;
};

SettingsUI getSettingsFrame();

struct WorldSettingsUI {
    std::string name = "worldsettings";

    Text* text;
    Button* back;

    Panel* panel;
    Button* begin;
};

WorldSettingsUI getWorldSettingsFrame();

struct LoadingUI {
    std::string name = "loading";

    Panel* panel;
    Text* text;
    Text* chunks;
	Text* animation;
};

LoadingUI getLoadingFrame();

struct InGameUI {
    std::string name = "ingame";

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
    std::string name = "minimap";
    Text* text;
    Text* seed;
};

MiniMapUI getMiniMapFrame();

struct BuildUI {
    std::string name = "build";

    Button* structureButton;
    Button* furnitureButton;
    Button* productionButton;
    Button* securityButton;
};

BuildUI getBuildFrame();

struct ProductionUI {
    std::string name = "production";

    Button* carpentry_bench;
	Button* stone_cutter;
	Button* furnace;
    Button* anvil;
};

ProductionUI getProductionFrame();

struct StructureUI {
    std::string name = "structure";

    Button* wood_wall;
    Button* stone_wall;
    Button* wood_fence;
    Button* stone_fence;
    Button* wood_floor;
};

StructureUI getStructureFrame();

struct FurnitureUI {
    std::string name = "furniture";
    Button* chair;
    Button* bed;
};

FurnitureUI getFurnitureFrame();

struct CarpentryBenchUI {
    std::string name = "carpentry";

    Text* text;
    Text* ingredients;

    Panel* panel;
    std::vector<Button*> craftable_items;

	Button* closeButton;
};

CarpentryBenchUI getCarpentryBenchFrame();

struct AnvilUI {
    std::string name = "anvil";
    Text* text;
    Panel* panel;
    std::vector<Button*> craftable_items;
    Button* closeButton;
};

AnvilUI getAnvilFrame();

struct GunBenchUI {
    std::string name = "gun";
    Text* text;
    Text* ingredients;
    Panel* panel;
    std::vector<Button*> craftable_items;
    Button* closeButton;
};

GunBenchUI getGunBenchFrame();

struct VillagerInfoUI {
    std::string name = "villager";

    Panel* infoPanel;

    Text* text;
    Text* job;

    Text* health;
};

VillagerInfoUI getVillagerInfoFrame();

struct HarvestUI {
    std::string name = "harvest";

	Panel* infoPanel;

    Text* text;

	std::vector<Text*> harvestable_item_texts;
    std::vector<Checkbox*> harvestable_items;
};

HarvestUI getHarvestFrame();

struct InfoUI {
    std::string name = "info";
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
    Alignment align
);

void setMode(Mode mode);
void configureInfo(std::string obj);