#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#include "Components.h"

class CameraSystem {
private:
    sf::View gameView;
    float smoothness = 0.1f; 

public:
    CameraSystem(int windowWidth, int windowHeight);
    void update(sf::RenderWindow& window, const std::vector<std::shared_ptr<Entity>>& entities, float deltaTime);
    sf::View& getView() { return gameView; }
    void move(float x, float y);
};