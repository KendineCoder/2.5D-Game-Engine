#include "EntityManager.h"

std::shared_ptr<Entity> EntityManager::createEntity() {
    auto entity = std::make_shared<Entity>();
    entities.push_back(entity);
    return entity;
}

std::shared_ptr<Entity> EntityManager::createEntity(const std::string& tag) {
    auto entity = createEntity();
    taggedEntities[tag].push_back(entity);
    return entity;
}

void EntityManager::deactivateEntity(const std::shared_ptr<Entity>& entity) {
    if (entity) {
        entity->setActive(false);
    }
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesByTag(const std::string& tag) {
    auto it = taggedEntities.find(tag);
    if (it != taggedEntities.end()) {
        return it->second;
    }
    return {};
}

std::shared_ptr<Entity> EntityManager::getEntityByTag(const std::string& tag) {
    auto entities = getEntitiesByTag(tag);
    return entities.empty() ? nullptr : entities.front();
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::getEntities() const {
    return entities;
}

void EntityManager::clear() {
    entities.clear();
    taggedEntities.clear();
}

void EntityManager::update(float deltaTime) {
    // First remove inactive entities from tagged lists
    for (auto& tagPair : taggedEntities) {
        auto& entityList = tagPair.second;
        entityList.erase(
            std::remove_if(entityList.begin(), entityList.end(),
                [](const auto& entity) {
                    return !entity || !entity->isActive();
                }),
            entityList.end()
        );
    }

    // Then remove from main entities list
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const auto& entity) {
                return !entity || !entity->isActive();
            }),
        entities.end()
    );
}
