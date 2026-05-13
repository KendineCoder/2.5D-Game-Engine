#include "BuildingSystem.h"
#include <iostream>

BuildingSystem::BuildingSystem(EntityManager& em, EntityFactory& ef, std::vector<std::vector<int>>& map, int w, int h)
    : entityManager(em), entityFactory(ef), worldMap(map), mapWidth(w), mapHeight(h) {}

sf::Vector2i BuildingSystem::worldToGrid(sf::Vector2f worldPos) {
    const float TILE_WIDTH = 256.0f;
    const float TILE_HEIGHT = 128.0f;
    const float GRID_OFFSET_X = 960.0f;
    const float GRID_OFFSET_Y = 400.0f;

    float adjX = worldPos.x - GRID_OFFSET_X;
    float adjY = worldPos.y - GRID_OFFSET_Y;

    float gridX = (adjX / (TILE_WIDTH / 2.0f) + adjY / (TILE_HEIGHT / 2.0f)) / 2.0f;
    float gridY = (adjY / (TILE_HEIGHT / 2.0f) - adjX / (TILE_WIDTH / 2.0f)) / 2.0f;

    return sf::Vector2i(static_cast<int>(std::round(gridX)), static_cast<int>(std::round(gridY)));
}

bool BuildingSystem::canPlaceBuilding(int startX, int startY, int width, int height) {
    int currentMapHeight = worldMap.size();
    int currentMapWidth = (currentMapHeight > 0) ? worldMap[0].size() : 0;

    if (startX < 0 || startY < 0 || startX + width > currentMapWidth || startY + height > currentMapHeight) {
        return false;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int currentTileId = worldMap[startY + y][startX + x];
            if (currentTileId == 2 || currentTileId == 6 || currentTileId >= 10) {
                return false;
            }
        }
    }
    return true;
}

bool BuildingSystem::buildStructure(int gridX, int gridY, int buildingId) {
   
    PrefabTemplate tmpl = entityFactory.getPrefab(buildingId);

    if (canPlaceBuilding(gridX, gridY, tmpl.gridWidth, tmpl.gridHeight)) {

      
        for (int y = 0; y < tmpl.gridHeight; ++y) {
            for (int x = 0; x < tmpl.gridWidth; ++x) {
                worldMap[gridY + y][gridX + x] = buildingId;
            }
        }

     
        entityFactory.createEntityAt(gridX, gridY, buildingId);
        std::cout << "Insa edildi: " << tmpl.name << std::endl;
        return true;
    }
    else {
        std::cout << "Uyari: Insaat alani dolu veya gecersiz!" << std::endl;
        return false;
    }
}