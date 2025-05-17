#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

struct Component {
    virtual ~Component() = default;
};

struct TransformComponent : public Component {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;
    float scale = 1.0f;
};

struct RenderComponent : public Component {
    sf::Sprite sprite;
    bool isVisible = true;
    int layerOrder = 0; // Used for sorting
};

struct CollisionComponent : public Component {
    float width = 0.0f;
    float height = 0.0f;
    bool isCollidable = true;
};

struct IsometricGridComponent : public Component {
    int gridX = 0;
    int gridY = 0;
    float tileWidth = 256.0f;
    float tileHeight = 128.0f;
};

struct TileComponent : public Component {
    int tileId = 0;
    bool isWalkable = true;
    int textureColumn = 0;
    int textureRow = 0;
    std::string name = "Unknown";
};

struct CameraComponent : public Component {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float zoom = 0.5f;
    bool followPlayer = true;
};