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

// Components.h dosyasındaki ilgili kısım

// DİKKAT: ": public Component" kısmını ekledik
struct AnimationComponent : public Component {
    int currentFrame = 0;
    int totalFrames = 8;
    float frameDuration = 0.1f;
    float elapsedTime = 0.0f;
    int currentRow = 4;
    bool isPlaying = false;
};

// DİKKAT: ": public Component" kısmını ekledik
struct MovementComponent : public Component {
    bool isMoving = false;
    sf::Vector2f startWorldPos;
    sf::Vector2f targetWorldPos;
    float moveDuration = 0.2f; // MOVEMENT_DELAY yerine doğrudan değer yazabilirsin
    float moveTimer = 0.0f;
};