#pragma once
#include "UIManager.h"
#include "UI.h"

enum struct Material {
    WOOD,
    STONE,
};

// Responsible for managing UI, Worlds, Settings, and Game States
class Game {
private:
    Game() {}
    UIManager uiManager;
    
    MainMenuUI mainMenuUI;
	SettingsUI settingsUI;
    WorldSettingsUI worldSettingsUI;
	LoadingUI loadingUI;
    InGameUI inGameUI;
	MiniMapUI miniMapUI;
    BuildUI buildUI;
    ProductionUI productionUI;
	FurnitureUI furnitureUI;
    StructureUI structureUI;
    CarpentryBenchUI carpentryBenchUI;
	AnvilUI anvilUI;
	GunBenchUI gunBenchUI;
    StockpileUI stockpileUI;
    VillagerInfoUI villagerInfoUI;
	HarvestUI harvestUI;
	PlantUI plantUI;
	InfoUI infoUI;

    LightManager lightManager;
	Material selectedMaterial = Material::WOOD;

    std::shared_ptr<Object> selectedBuildItem;
    Object* selectedCraftItem;

public:

    std::string selectedPlantItem;

    static Game& getInstance() {
        static Game instance;
        return instance;
    }

    void setSelectedMaterial(Material material) {
        selectedMaterial = material;
    }

    void initUI() {
        mainMenuUI = getMainMenuFrame();
		settingsUI = getSettingsFrame();
        worldSettingsUI = getWorldSettingsFrame();
		loadingUI = getLoadingFrame();
        inGameUI = getInGameFrame();
		miniMapUI = getMiniMapFrame();
        buildUI = getBuildFrame();
        productionUI = getProductionFrame();
		furnitureUI = getFurnitureFrame();
        structureUI = getStructureFrame();
        carpentryBenchUI = getCarpentryBenchFrame();
		anvilUI = getAnvilFrame();
		gunBenchUI = getGunBenchFrame();
        stockpileUI = getStockpileFrame();
        villagerInfoUI = getVillagerInfoFrame();
        infoUI = getInfoFrame();
		harvestUI = getHarvestFrame();
		plantUI = getPlantFrame();
    }

    MainMenuUI& getMainMenuUI() {
        return mainMenuUI;
    }

    SettingsUI& getSettingsUI() {
        return settingsUI;
	}

    WorldSettingsUI& getWorldSettingsUI() {
        return worldSettingsUI;
    }

    LoadingUI& getLoadingUI() {
        return loadingUI;
	}

    InGameUI& getInGameUI() {
        return inGameUI;
    }

    MiniMapUI& getMiniMapUI() {
        return miniMapUI;
    }

    BuildUI& getBuildUI() {
        return buildUI;
    }

    ProductionUI& getProductionUI() {
        return productionUI;
    }

    FurnitureUI& getFurnitureUI() {
        return furnitureUI;
	}

    StructureUI& getStructureUI() {
        return structureUI;
    }

    CarpentryBenchUI& getCarpentryBenchUI() {
        return carpentryBenchUI;
    }

    AnvilUI& getAnvilUI() {
        return anvilUI;
	}

    GunBenchUI& getGunBenchUI() {
        return gunBenchUI;
    }

    StockpileUI& getStockpileUI() {
        return stockpileUI;
    }

    VillagerInfoUI& getVillagerInfoUI() {
        return villagerInfoUI;
    }

    HarvestUI& getHarvestUI() {
        return harvestUI;
	}

    PlantUI& getPlantUI() {
        return plantUI;
    }

    InfoUI& getInfoUI() {
        return infoUI;
	}

    Material getSelectedMaterial() const {
        return selectedMaterial;
	}

    UIManager& getUIManager() {
        return uiManager;
    }

    LightManager& getLightManager() {
        return lightManager;
    }

    std::shared_ptr<Object> getBuildItem() {
        return selectedBuildItem;
    }

    void setBuildItem(const std::string& name) {
        selectedBuildItem = ObjectRegistry::getInstance().get(name);
    }

    Object* getCraftItem() {
        return selectedCraftItem;
    }

    void setCraftItem(std::string name) {
        selectedCraftItem = ObjectRegistry::getInstance().get(name).get();
    }
    
};