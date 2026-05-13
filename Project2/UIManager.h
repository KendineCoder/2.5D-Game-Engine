#pragma once
#include <SFML/Graphics.hpp>
#include "BuildingSystem.h"
#include "EntityFactory.h" 

class UIManager {
private:
    int selectedBuildingId = -1;
    const int UI_HEIGHT = 100;
    const int BUTTON_WIDTH = 100;

public:
    int getSelectedBuildingId() const { return selectedBuildingId; }
    bool handleMouseClick(int screenX, int screenY, int screenHeight, EntityFactory& entityFactory);
    void drawUI(sf::RenderWindow& window, EntityFactory& entityFactory);
    void deselectBuilding() { selectedBuildingId = -1; }
    void drawGhostBuilding(sf::RenderWindow& window, BuildingSystem& buildingSystem, EntityFactory& entityFactory);
};