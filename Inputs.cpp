#include "Inputs.h"
#include "Job.h"
#include "Item.h"
#include "Tile.h"

double lastTime = glfwGetTime();
int nbFrames = 0;
std::string fps;


void processInput(GLFWwindow* window) {

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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !clicked) {

		std::cout << getTileRef(mouseTileX, mouseTileY).walkable << std::endl;

        if (buildMode || harvestMode || plantMode || stockpileMode) {
            if (!placing) {
                // First click — start placement
                corner = { mouseTileX, mouseTileY };
                placing = true;
            }
            else {

                int left = std::min(corner.first, mouseTileX);
                int right = std::max(corner.first, mouseTileX);
                int top = std::min(corner.second, mouseTileY);
                int bottom = std::max(corner.second, mouseTileY);

                for (int x = left; x <= right; x++) {
                    for (int y = top; y <= bottom; y++) {

						Tile& tile = getTileRef(x, y);

                        if (buildMode) {
                            if (x == left || x == right || y == top || y == bottom) {
                                JobManager::JobList.push_back(new Build(nullptr, JobType::Builder, &wall, x, y));
                            }
                            
                        }
                        else if (harvestMode) {
                            if (tile.items.size() != 0) {
								JobType jobType;
                                Item* item = nullptr;

                                if (tile.containsItem(tree)) {
									jobType = JobType::Lumberjack;
									item = &tree;
                                }
                                else if (tile.containsItem(tree2)) {
                                    jobType = JobType::Lumberjack;
                                    item = &tree2;
                                }
                                else if (tile.containsItem(rock)) {
                                    jobType = JobType::Miner;
                                    item = &rock;
                                }
                                else if (tile.containsItem(flower)) {
                                    jobType = JobType::Farmer;
                                    item = &flower;
                                }

                                if (item) {
                                    Job* job = new HarvestTile(nullptr, jobType, item, x, y);
                                    job->priority = 10;
                                    JobManager::JobList.push_back(job);
                                }
								
                            }
                        }
                        else if (plantMode) {
							JobManager::JobList.push_back(new Plant(nullptr, JobType::Farmer, &wheat, x, y));
                        }
                        else if (stockpileMode) {
                            tile.items.clear();
                            tile.addItem(std::make_unique<Item>(stockPile));
                            Stockpile stockpile({x, y});
                            stockpileTiles.push_back(stockpile);
                        }
                    }
                }


                placing = false;
            }
        }
        else {
            placing = false;
        }

        clicked = true;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        clicked = false;
    }


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!viewMiniMap) viewMiniMap = true;
        else if (viewMiniMap) viewMiniMap = false;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!viewHeightMap) viewHeightMap = true;
        else if (viewHeightMap) viewHeightMap = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        xTextSpacing++;
        yTextSpacing++;
        fontSize += 0.02f;

        xFrustum = scrWidth / xTextSpacing;
        yFrustum = scrHeight / yTextSpacing;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        xTextSpacing--;
        yTextSpacing--;
        fontSize -= 0.02f;

        xFrustum = scrWidth / xTextSpacing;
        yFrustum = scrHeight / yTextSpacing;
    }

    for (auto& i : ui.UIButtons) {
        i.update(mouseX / xTextSpacing, mouseY / yTextSpacing, clicked);
    }


    

    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >= 1.0) {
        fps = "FPS: " + std::to_string(nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
        
    }

    

    int num = 1;
	
    for (char c : fps) {
        ui.UI[1][num] = c;
        num++;
    }

    int row = 0;

    for (int i = 0; i < 16; i++) {
        ui.UI[0][i + 64] = ' ';
        ui.UI[1][i + 64] = ' ';
        ui.UI[2][i + 64] = ' ';
    }

    int i = 0;
    std::string villagerCountStr = std::to_string(Creature::allCreatures.size());
    for (char c : villagerCountStr) {
        ui.UI[0][i + 13] = c;
        i++;
    }

	Tile& tile = getTileRef(mouseTileX, mouseTileY);
    i = 0;
    for (char c : tile.typeString) {
        ui.UI[row][i + 64] = c;
        i++;
    }
    i = 0;
    row++;

    for (auto& item : tile.items) {
		std::string itemStr = item->name;
        
        if (Crop* crop = dynamic_cast<Crop*>(item.get())) {
            float growth = static_cast<float>(crop->growStage + 1) / static_cast<float>(crop->stages.size());

            // Clamp to avoid weird values
            if (growth > 1.0f) growth = 1.0f;
            if (growth < 0.0f) growth = 0.0f;

            int percent = static_cast<int>(growth * 100.0f);
            itemStr += " (" + std::to_string(percent) + "% Grown)";
        }
        for (char c : itemStr) {
            ui.UI[row][i + 64] = c;
            i++;
		}
        i = 0;
        row++;
    }


    for (Villager* i : Villager::allVillagers) {
        if (i->xPos == mouseTileX && i->yPos == mouseTileY) {
            
            std::string nameStr = i->firstname + " " + i->lastname + " - " + jobTypeToString(i->jobType);
			std::string occupStr = "Job: " + jobTypeToString(i->jobType);
            int j = 0;
            for (char c : nameStr) {
                ui.UI[row][j + 64] = c;
                j++;
                if (j > 63) {
					break;
                }
            }
            row++;
            /*j = 0;
            row++;
            for (char c : occupStr) {
				ui.UI[row][j + 64] = c;
				j++;
            }*/
        }
    }
    
}