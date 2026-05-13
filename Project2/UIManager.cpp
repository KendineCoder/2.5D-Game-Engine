#include "UIManager.h"
#include <algorithm> 

bool UIManager::handleMouseClick(int screenX, int screenY, int screenHeight, EntityFactory& entityFactory) {

    if (screenY > screenHeight - UI_HEIGHT) {
        int buttonIndex = screenX / BUTTON_WIDTH;
        int currentIndex = 0;

        for (const auto& pair : entityFactory.getAllPrefabs()) {
            if (pair.first >= 10) {
                if (currentIndex == buttonIndex) {
                    selectedBuildingId = pair.first;
                    return true; 
                }
                currentIndex++;
            }
        }
        return true; 
    }
    return false; 
}

void UIManager::drawUI(sf::RenderWindow& window, EntityFactory& entityFactory) {

    sf::RectangleShape uiPanel(sf::Vector2f(window.getSize().x, UI_HEIGHT));
    uiPanel.setPosition(0, window.getSize().y - UI_HEIGHT);
    uiPanel.setFillColor(sf::Color(40, 40, 40, 220)); 
    window.draw(uiPanel);

    int buttonIndex = 0;

    for (const auto& pair : entityFactory.getAllPrefabs()) {
     
        if (pair.first < 10) continue;

        const PrefabTemplate& tmpl = pair.second;
        float buttonX = buttonIndex * BUTTON_WIDTH;
        float buttonY = window.getSize().y - UI_HEIGHT;

     
        if (selectedBuildingId == pair.first) {
            sf::RectangleShape highlight(sf::Vector2f(BUTTON_WIDTH, UI_HEIGHT));
            highlight.setPosition(buttonX, buttonY);
            highlight.setFillColor(sf::Color(0, 200, 0, 100)); 
            window.draw(highlight);
        }

        if (tmpl.texture) {
            sf::Sprite icon;
            icon.setTexture(*tmpl.texture);
            icon.setTextureRect(tmpl.textureRect);

            float scaleX = (BUTTON_WIDTH - 20.0f) / tmpl.textureRect.width;
            float scaleY = (UI_HEIGHT - 20.0f) / tmpl.textureRect.height;
            float scale = std::min(scaleX, scaleY);

            icon.setScale(scale, scale);

           
            icon.setPosition(
                buttonX + (BUTTON_WIDTH - tmpl.textureRect.width * scale) / 2.0f,
                buttonY + (UI_HEIGHT - tmpl.textureRect.height * scale) / 2.0f
            );

            window.draw(icon);
        }
        buttonIndex++;
    }
}

void UIManager::drawGhostBuilding(sf::RenderWindow& window, BuildingSystem& buildingSystem, EntityFactory& entityFactory) {
    if (selectedBuildingId == -1) return;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);


    if (mousePos.y > window.getSize().y - UI_HEIGHT) return;

    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    sf::Vector2i gridPos = buildingSystem.worldToGrid(worldPos);

    
    PrefabTemplate tmpl = entityFactory.getPrefab(selectedBuildingId);

    if (tmpl.texture) {
        sf::Sprite ghost;
        ghost.setTexture(*tmpl.texture);
        ghost.setTextureRect(tmpl.textureRect);
        ghost.setOrigin(tmpl.origin); 

      
        sf::Vector2f isoPos = entityFactory.gridToWorld(gridPos.x, gridPos.y);
        ghost.setPosition(isoPos);

        // Oraya inşa edilebiliyor mu kontrol et
        if (buildingSystem.canPlaceBuilding(gridPos.x, gridPos.y, tmpl.gridWidth, tmpl.gridHeight)) {
            ghost.setColor(sf::Color(0, 255, 0, 150)); 
        }
        else {
            ghost.setColor(sf::Color(255, 0, 0, 150));
        }

        window.draw(ghost);
    }
}