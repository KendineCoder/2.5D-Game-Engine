#include "CameraSystem.h"
#include <iostream>

CameraSystem::CameraSystem(int windowWidth, int windowHeight) {

    gameView.setSize(windowWidth, windowHeight);
    gameView.setCenter(windowWidth / 2, windowHeight / 2);
    gameView.zoom(3);
}

void CameraSystem::update(sf::RenderWindow& window, const std::vector<std::shared_ptr<Entity>>& entities, float deltaTime) {
 
    std::shared_ptr<Entity> cameraEntity = nullptr;

    for (const auto& entity : entities) {
        if (entity->hasComponent<CameraComponent>()) {
            cameraEntity = entity;
            break;
        }
    }

    if (!cameraEntity) return;

    auto camera = cameraEntity->getComponent<CameraComponent>();

   
    if (camera->followPlayer) {
        std::shared_ptr<Entity> playerEntity = nullptr;

        for (const auto& entity : entities) {
            if (entity->hasComponent<TransformComponent>() &&
                entity->hasComponent<RenderComponent>() &&
                entity == cameraEntity) { 
                playerEntity = entity;
                break;
            }
        }

        if (playerEntity) {
            auto transform = playerEntity->getComponent<TransformComponent>();

            // Calculate target camera position
            float targetX = transform->x + camera->offsetX;
            float targetY = transform->y + camera->offsetY;

            // Get current view center
            sf::Vector2f currentCenter = gameView.getCenter();

            // Smoothly interpolate between current and target position
            float newX = currentCenter.x + (targetX - currentCenter.x) * smoothness;
            float newY = currentCenter.y + (targetY - currentCenter.y) * smoothness;

            gameView.setCenter(newX, newY);

            window.setView(gameView);
        }
    }
}

void CameraSystem::move(float x, float y) {
    gameView.move(x, y);
}
