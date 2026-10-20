#include "Jobs/Jobs/AddToFurnace.h"
#include "World/World.h"
#include "Jobs/JobUtils.h"
#include "Entities/Crop.h"


void CropManager::updateCropSystem(float deltaTime) {
    auto view = registry.view<Position, Name, Renderable, Crop>();
    for (auto [entity, pos, name, renderable, crop] : view.each()) {
        crop.growthClock += deltaTime;
        if (crop.growthClock >= crop.currentGrowthTime) {

            // hard cap in case the death thing isnt added for a plant
            if (crop.growthStage >= crop.growthStages.size() - 1 || crop.state == CropState::DEAD) {
                crop.growthStage = crop.growthStageMax;
                continue;
            }

            crop.growthClock = 0.0f;
            crop.growthStage++;

            const float variance = getRandomFloat(-0.25f, 0.25f);
            crop.currentGrowthTime = crop.growthTime * (1.0f + variance);

            if (crop.growthStage == crop.mature_stage) {
                crop.state = CropState::MATURED;
                registry.emplace<Matured>(entity);
                crop.currentGrowthTime *= 2;
            }
            else if (crop.growthStage == crop.death_age) {
                registry.erase<Matured>(entity);
                crop.state = CropState::DEAD;
            }
            else {
                crop.state = CropState::SPROUT;
            }

            const auto& currentStage = crop.growthStages[crop.growthStage];
            renderable.character = currentStage.character;
            renderable.color = currentStage.color;
        }
    }
}