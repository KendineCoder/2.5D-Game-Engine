#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
class Entity;

struct Component {
    virtual ~Component() = default;
};

struct TransformComponent : public Component {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;

    float scaleX = 1.0f;
    float scaleY = 1.0f;
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


struct AnimationComponent : public Component {
    int currentRow = 0;
    int currentFrame = 0;
    int totalFrames = 4;
    float frameDuration = 0.1f;
    float elapsedTime = 0.0f;
    bool isPlaying = false;

    int frameWidth = 64;
    int frameHeight = 64;
};

struct MovementComponent : public Component {
    bool isMoving = false;
    float moveTimer = 0.0f;
    float moveDuration = 0.3f;
    sf::Vector2f startWorldPos;
    sf::Vector2f targetWorldPos;

    std::vector<sf::Vector2i> path; // Gidilecek adımlar listesi
    int currentPathIndex = 0;       
    bool isAttacking = false;
};

// Düşman Yapay Zekası
struct AIComponent : public Component {
    float thinkTimer = 0.0f;
    float thinkInterval = 1.0f; 

    float attackTimer = 0.0f;
    float attackRate = 1.5f;
    int attackDamage = 15;
};

struct HealthComponent : public Component {
    float maxHealth = 100.0f;
    float currentHealth = 100.0f;
    float deathTimer = 0.0f;
    float decayTime = 5.0f;
};

struct StaminaComponent : public Component {
    float maxStamina = 100.0f;
    float currentStamina = 100.0f;
    float attackCost = 30.0f;     
    float regenRate = 15.0f;     
};
struct EconomyComponent : public Component {
    int gold = 100; 
};

struct TurretComponent : public Component {
    float attackRange = 350.0f;
    float fireRate = 1.5f;      
    float fireTimer = 0.0f;
    int damage = 35;            
};

struct ProjectileComponent : public Component {
    std::shared_ptr<Entity> target;
    float speed = 800.0f;       
    int damage = 0;
};
struct ProductionComponent : public Component {
    float interval = 5.0f;     
    float timer = 0.0f;        
    int amount = 10;            
};