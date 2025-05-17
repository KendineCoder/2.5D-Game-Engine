#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include "components.h"

class RenderSystem {
public:
    void update(sf::RenderWindow& window, const std::vector<std::shared_ptr<Entity>>& entities);
};