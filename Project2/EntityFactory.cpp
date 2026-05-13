#include "EntityFactory.h"
#include <iostream>

EntityFactory::EntityFactory(EntityManager& em) : entityManager(em) {}

sf::Vector2f EntityFactory::gridToWorld(int gridX, int gridY) {
    float worldX = (gridX - gridY) * (TILE_WIDTH / 2.0f);
    float worldY = (gridX + gridY) * (TILE_HEIGHT / 2.0f);
    return sf::Vector2f(worldX + GRID_OFFSET_X, worldY + GRID_OFFSET_Y);
}

void EntityFactory::registerPrefab(int id, const std::string& name, bool walkable, int w, int h, sf::Texture& tex, int rX, int rY, int rW, int rH, float oX, float oY, int layer, int cost = 0) {
    PrefabTemplate p;
    p.id = id;
    p.name = name;
    p.isWalkable = walkable;
    p.gridWidth = w;
    p.gridHeight = h;
    p.texture = &tex;
    p.textureRect = sf::IntRect(rX, rY, rW, rH);
    p.origin = sf::Vector2f(oX, oY);
    p.layerOrder = layer;
    p.cost = cost;
    prefabs[id] = p;
}

PrefabTemplate EntityFactory::getPrefab(int id) {
    auto it = prefabs.find(id);
    if (it != prefabs.end()) return it->second;
    return prefabs[0]; // varsayılan ID 0 Çim 
}

std::shared_ptr<Entity> EntityFactory::createEntityAt(int gridX, int gridY, int prefabId) {
    PrefabTemplate tmpl = getPrefab(prefabId);

    std::string tagToAssign = tmpl.isWalkable ? "Tile" : "Solid";
    if (prefabId == 20) {
        tagToAssign = "Turret";
    }
    if (prefabId == 10) {
        tagToAssign = "Tavern";
    }
    auto entity = entityManager.createEntity(tagToAssign);
    if (prefabId == 10) {
        auto prod = std::make_shared<ProductionComponent>();
        prod->interval = 5.0f;
        prod->amount = 10;
        entity->addComponent(prod);
    }
    auto isoGrid = std::make_shared<IsometricGridComponent>();
    isoGrid->gridX = gridX;
    isoGrid->gridY = gridY;
    isoGrid->tileWidth = TILE_WIDTH;
    isoGrid->tileHeight = TILE_HEIGHT;
    entity->addComponent(isoGrid);

    auto transform = std::make_shared<TransformComponent>();
    sf::Vector2f worldPos = gridToWorld(gridX, gridY);
    transform->x = worldPos.x;
    transform->y = worldPos.y;
    entity->addComponent(transform);

    if (tmpl.texture) {
        auto render = std::make_shared<RenderComponent>();
        render->sprite.setTexture(*(tmpl.texture));
        render->sprite.setTextureRect(tmpl.textureRect);
        render->sprite.setOrigin(tmpl.origin);
        render->layerOrder = tmpl.layerOrder;
        entity->addComponent(render);
    }

    if (prefabId == 20) {
        auto turret = std::make_shared<TurretComponent>();
        entity->addComponent(turret);

        auto health = std::make_shared<HealthComponent>();
        health->maxHealth = 200.0f;
        health->currentHealth = 200.0f;
        entity->addComponent(health);
    }

    auto tileComp = std::make_shared<TileComponent>();
    tileComp->tileId = tmpl.id;
    tileComp->isWalkable = tmpl.isWalkable;
    tileComp->name = tmpl.name;
    entity->addComponent(tileComp);

    return entity;
}