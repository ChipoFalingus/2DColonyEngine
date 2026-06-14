#pragma once
#include "UIManager.h"
#include "UI.h"

#include "Heat.h"

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
	VillagerListUI villagerListUI;
    BuildUI buildUI;
    ProductionUI productionUI;
	FurnitureUI furnitureUI;
    TemperatureUI temperatureUI;
    StructureUI structureUI;
    CarpentryBenchUI carpentryBenchUI;
	StoneCutterUI stoneCutterUI;
	AnvilUI anvilUI;
	GunBenchUI gunBenchUI;
    StockpileUI stockpileUI;
    VillagerInfoUI villagerInfoUI;
	HarvestUI harvestUI;
	PlantUI plantUI;
	InfoUI infoUI;

    LightManager lightManager;
	HeatManager heatManager;
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
		villagerListUI = getVillagerListFrame();
        buildUI = getBuildFrame();
        productionUI = getProductionFrame();
		furnitureUI = getFurnitureFrame();
        temperatureUI = getTemperatureFrame();
        structureUI = getStructureFrame();
        carpentryBenchUI = getCarpentryBenchFrame();
		stoneCutterUI = getStoneCutterFrame();
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

    VillagerListUI& getVillagerListUI() {
        return villagerListUI;
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

    TemperatureUI& getTemperatureUI() {
        return temperatureUI;
    }

    StructureUI& getStructureUI() {
        return structureUI;
    }

    CarpentryBenchUI& getCarpentryBenchUI() {
        return carpentryBenchUI;
    }

    StoneCutterUI& getStoneCutterUI() {
        return stoneCutterUI;
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

    HeatManager& getHeatManager() {
        return heatManager;
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