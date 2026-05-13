#include "RenderSystem.h"
#include <iostream>
#include <algorithm>

void RenderSystem::update(sf::RenderWindow& window, const std::vector<std::shared_ptr<Entity>>& entities) {

    std::vector<std::shared_ptr<Entity>> sortedEntities = entities;

    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
            auto renderA = a->getComponent<RenderComponent>();
            auto renderB = b->getComponent<RenderComponent>();

            
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

   
    for (const auto& entity : sortedEntities) {
        if (!entity->isActive()) continue;

        auto render = entity->getComponent<RenderComponent>();
        auto transform = entity->getComponent<TransformComponent>();

        if (render && render->isVisible && transform) {
            render->sprite.setPosition(transform->x, transform->y);
            render->sprite.setRotation(transform->rotation);
            render->sprite.setScale(transform->scaleX, transform->scaleY);

    
            if (transform && render) {

                window.draw(render->sprite);

                auto health = entity->getComponent<HealthComponent>();

                if (health && health->currentHealth > 0) {
                    auto anim = entity->getComponent<AnimationComponent>();

                 
                    float barWidth = 100.0f;
                    float barHeight = 20.0f;

                    float barX = transform->x - (barWidth / 2.0f);
                    float barY = transform->y - 250.0f;

                 
                    sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
                    bg.setPosition(barX, barY);
                    bg.setFillColor(sf::Color(40, 0, 0, 200));
                    window.draw(bg);

                    float hpPercent = health->currentHealth / health->maxHealth;
                    if (hpPercent < 0) hpPercent = 0.0f; 

                    sf::RectangleShape fill(sf::Vector2f(barWidth * hpPercent, barHeight));
                    fill.setPosition(barX, barY);

                    
                    fill.setFillColor(sf::Color(255, 50, 50)); 
                    

                    window.draw(fill);
                }
            }
        }
    }
}