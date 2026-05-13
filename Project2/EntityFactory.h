#pragma once
#include "EntityManager.h"
#include "Components.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

struct PrefabTemplate {
    int id;
    std::string name;
    bool isWalkable;       
    int gridWidth = 1;     
    int gridHeight = 1;

    sf::Texture* texture;
    sf::IntRect textureRect;
    sf::Vector2f origin;
    int layerOrder = 0;
    int cost = 0;
};

class EntityFactory {
private:
    EntityManager& entityManager; 
    std::map<int, PrefabTemplate> prefabs; 

   
    const float TILE_WIDTH = 256.0f;
    const float TILE_HEIGHT = 128.0f;
    const float GRID_OFFSET_X = 960.0f;
    const float GRID_OFFSET_Y = 400.0f;

    

public:
    EntityFactory(EntityManager& em);
    sf::Vector2f gridToWorld(int gridX, int gridY);
 
    void registerPrefab(int id, const std::string& name, bool walkable, int w, int h, sf::Texture& tex, int rX, int rY, int rW, int rH, float oX, float oY, int layer, int cost);
    const std::map<int, PrefabTemplate>& getAllPrefabs() const { return prefabs; }
    PrefabTemplate getPrefab(int id);
    std::shared_ptr<Entity> createEntityAt(int gridX, int gridY, int prefabId);
};