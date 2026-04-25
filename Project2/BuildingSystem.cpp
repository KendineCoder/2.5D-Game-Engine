#include "BuildingSystem.h"

BuildingSystem::BuildingSystem(EntityManager& manager, std::vector<std::vector<int>>& map, int width, int height)
    : entityManager(manager), worldMap(map), mapWidth(width), mapHeight(height) {
    initializeBuildings();
}

void BuildingSystem::initializeBuildings() {
    // 1x1 Duvar
    BuildingTemplate wall;
    wall.id = 6;
    wall.name = "Wall";
    wall.gridWidth = 1;
    wall.gridHeight = 1;
    buildingTemplates[6] = wall;

    // 2x2 Taverna vs. eklenebilir
    BuildingTemplate tavern;
    tavern.id = 10;
    tavern.name = "Tavern";
    tavern.gridWidth = 2;
    tavern.gridHeight = 2;
    buildingTemplates[10] = tavern;
}

sf::Vector2f BuildingSystem::gridToWorld(int gridX, int gridY) {
    float worldX = (gridX - gridY) * (TILE_WIDTH / 2);
    float worldY = (gridX + gridY) * (TILE_HEIGHT / 2);
    return sf::Vector2f(worldX + GRID_OFFSET_X, worldY + GRID_OFFSET_Y);
}

sf::Vector2i BuildingSystem::worldToGrid(sf::Vector2f worldPos) {
    float adjustedY = worldPos.y + (TILE_HEIGHT / 2.0f);
    float x = worldPos.x - GRID_OFFSET_X;
    float y = adjustedY - GRID_OFFSET_Y;

    int gridX = std::round((x / TILE_WIDTH) + (y / TILE_HEIGHT));
    int gridY = std::round((y / TILE_HEIGHT) - (x / TILE_WIDTH));

    return sf::Vector2i(gridX, gridY);
}

bool BuildingSystem::canPlaceBuilding(int startX, int startY, int width, int height) {
    if (startX < 0 || startY < 0 || startX + width > mapWidth || startY + height > mapHeight) {
        return false;
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (worldMap[startY + y][startX + x] != 0) { // 0: Boş çim
                return false;
            }
        }
    }
    return true;
}

void BuildingSystem::buildStructure(int gridX, int gridY, int buildingId) {
    auto it = buildingTemplates.find(buildingId);
    if (it == buildingTemplates.end()) return; // Bina bulunamadı

    BuildingTemplate tmpl = it->second;

    if (!canPlaceBuilding(gridX, gridY, tmpl.gridWidth, tmpl.gridHeight)) {
        std::cout << "Alan dolu, " << tmpl.name << " insa edilemedi!" << std::endl;
        return;
    }

    // Haritayı güncelle ve eski çimleri sil
    for (int y = 0; y < tmpl.gridHeight; ++y) {
        for (int x = 0; x < tmpl.gridWidth; ++x) {
            int currentX = gridX + x;
            int currentY = gridY + y;

            worldMap[currentY][currentX] = 7; // 7: Dolu Alan

            // O kordinattaki eski Entity'leri bul ve sil
            for (auto& entity : entityManager.getEntities()) {
                if (entity->hasComponent<TileComponent>() && entity->hasComponent<IsometricGridComponent>()) {
                    auto gridComp = entity->getComponent<IsometricGridComponent>();
                    if (gridComp->gridX == currentX && gridComp->gridY == currentY) {
                        entityManager.deactivateEntity(entity);
                    }
                }
            }
        }
    }

    // Yeni binayı Entity olarak yarat
    auto buildingEntity = entityManager.createEntity("Building");

    auto isoGrid = std::make_shared<IsometricGridComponent>();
    isoGrid->gridX = gridX;
    isoGrid->gridY = gridY;
    buildingEntity->addComponent(isoGrid);

    auto transform = std::make_shared<TransformComponent>();
    sf::Vector2f worldPos = gridToWorld(gridX, gridY);
    transform->x = worldPos.x;
    transform->y = worldPos.y;
    buildingEntity->addComponent(transform);

    auto render = std::make_shared<RenderComponent>();
    // render->sprite.setTexture(...); // Görseli ayarla
    render->layerOrder = 2;
    buildingEntity->addComponent(render);

    std::cout << tmpl.name << " insa edildi!" << std::endl;
}