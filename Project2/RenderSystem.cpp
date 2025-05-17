#include "RenderSystem.h"
#include <iostream>
#include <algorithm>

void RenderSystem::update(sf::RenderWindow& window, const std::vector<std::shared_ptr<Entity>>& entities) {

    std::vector<std::shared_ptr<Entity>> sortedEntities = entities;

    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
            auto renderA = a->getComponent<RenderComponent>();
            auto renderB = b->getComponent<RenderComponent>();

            // If both objects exist and have different layers
            if (renderA && renderB && renderA->layerOrder != renderB->layerOrder) {
                // Sort by layer
                if (renderA->layerOrder < renderB->layerOrder) {
                    return true;
                }
                else {
                    return false;
                }
            }

            // Then sort by Y position
            auto transformA = a->getComponent<TransformComponent>();
            auto transformB = b->getComponent<TransformComponent>();
            if (transformA && transformB) {
                return transformA->y < transformB->y;
            }

            return false;
        });

    // Render all visible entities
    for (const auto& entity : sortedEntities) {
        if (!entity->isActive()) continue;

        auto render = entity->getComponent<RenderComponent>();
        auto transform = entity->getComponent<TransformComponent>();

        if (render && render->isVisible && transform) {
            render->sprite.setPosition(transform->x, transform->y);
            render->sprite.setRotation(transform->rotation);
            render->sprite.setScale(transform->scale, transform->scale);

            window.draw(render->sprite);
        }
    }
}