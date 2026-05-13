#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "EntityFactory.h" 

class BuildingSystem {
private:
    EntityManager& entityManager;
    EntityFactory& entityFactory; // Artık her şeyi Fabrikaya soracak
    std::vector<std::vector<int>>& worldMap;
    int mapWidth;
    int mapHeight;

public:
    BuildingSystem(EntityManager& em, EntityFactory& ef, std::vector<std::vector<int>>& map, int w, int h);

    sf::Vector2i worldToGrid(sf::Vector2f worldPos);
    bool canPlaceBuilding(int startX, int startY, int width, int height);
    bool buildStructure(int gridX, int gridY, int buildingId);
};