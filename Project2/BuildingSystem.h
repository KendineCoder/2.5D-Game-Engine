#pragma once
#include "EntityManager.h"
#include "Components.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>

// Bina şablonu
struct BuildingTemplate {
    int id;
    std::string name;
    int gridWidth;
    int gridHeight;
    // Gerekirse textureRect eklenebilir
};

class BuildingSystem {
private:
    EntityManager& entityManager; // Referans olarak alıyoruz
    std::vector<std::vector<int>>& worldMap; // Haritayı değiştirebilmek için referans
    int mapWidth;
    int mapHeight;
    std::map<int, BuildingTemplate> buildingTemplates;

    // Harita offset ve tile boyutları (main'den buraya taşıdık veya parametre aldık)
    const float TILE_WIDTH = 256.0f;
    const float TILE_HEIGHT = 128.0f;
    const float GRID_OFFSET_X = 960.0f;
    const float GRID_OFFSET_Y = 400.0f;

    sf::Vector2f gridToWorld(int gridX, int gridY);

public:
    // Yapıcı metot (Constructor)
    BuildingSystem(EntityManager& manager, std::vector<std::vector<int>>& map, int width, int height);

    void initializeBuildings();
    sf::Vector2i worldToGrid(sf::Vector2f worldPos);
    bool canPlaceBuilding(int startX, int startY, int width, int height);
    void buildStructure(int gridX, int gridY, int buildingId);
};