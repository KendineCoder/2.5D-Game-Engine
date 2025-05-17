#pragma once

#include "Entity.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class EntityManager {
public:
    std::shared_ptr<Entity> createEntity();
    std::shared_ptr<Entity> createEntity(const std::string& tag);

    void deactivateEntity(const std::shared_ptr<Entity>& entity);

    template<typename T>
    std::vector<std::shared_ptr<Entity>> getEntitiesWithComponent();

    // Get entities by tag
    std::vector<std::shared_ptr<Entity>> getEntitiesByTag(const std::string& tag);

    // Get first entity with a specific tag
    std::shared_ptr<Entity> getEntityByTag(const std::string& tag);

    // Get all entities
    const std::vector<std::shared_ptr<Entity>>& getEntities() const;

    // Clear all entities
    void clear();

    // Update all entities
    void update(float deltaTime);

private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Entity>>> taggedEntities;
};

template<typename T>
std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesWithComponent() {
    std::vector<std::shared_ptr<Entity>> result;
    for (const auto& entity : entities) {
        if (entity->hasComponent<T>()) {
            result.push_back(entity);
        }
    }
    return result;
}
