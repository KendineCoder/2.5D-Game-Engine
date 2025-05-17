#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "Components.h"

class Entity {
public:
    void setActive(bool isActive) { active = isActive; }
    bool isActive() const { return active; }

    template<typename T>
    void addComponent(std::shared_ptr<T> component) {
        components[std::type_index(typeid(T))] = component;
    }

    template<typename T>
    std::shared_ptr<T> getComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent() {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
    bool active = true;
};