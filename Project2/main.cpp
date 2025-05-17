#include "EntityManager.h"
#include "Components.h"
#include "GameWindow.h"
#include "RenderSystem.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "CameraSystem.h"

class Game {
private:
    EntityManager entityManager;
    GameWindow window;
    CameraSystem cameraSystem;
    RenderSystem renderSystem;
    float deltaTime;
    std::chrono::steady_clock::time_point lastFrameTime;

    // Textures
    sf::Texture playerTexture;
    sf::Texture tilesTexture;

    // Constants for isometric grid
    const float TILE_WIDTH = 256.0f;
    const float TILE_HEIGHT = 128.0f;
    const float GRID_OFFSET_X = 960.0f; 
    const float GRID_OFFSET_Y = 400.0f;

    float movementCooldown = 0.0f;
    const float MOVEMENT_DELAY = 0.2f; 


    std::unordered_map<int, TileComponent> tileTypes;

    // World map
    std::vector<std::vector<int>> worldMap;
    int mapWidth = 40;
    int mapHeight = 40;

public:
    Game() : window(1280, 720, "Isometric Game"),
        lastFrameTime(std::chrono::steady_clock::now()),
        cameraSystem(1920, 1080) {

        // Load textures with error handling
        if (!playerTexture.loadFromFile("player.png")) {
            std::cerr << "Failed to load player texture!" << std::endl;
            playerTexture.create(128, 256);
            sf::Image img;
            img.create(128, 256, sf::Color::White);
            playerTexture.update(img);
        }

        if (!tilesTexture.loadFromFile("tiles.png")) {
            std::cerr << "Failed to load tiles texture!" << std::endl;
            tilesTexture.create(256, 256);
            sf::Image img;
            img.create(256, 256, sf::Color::Green);
            tilesTexture.update(img);
        }

        window.setKeyPressCallback([this](sf::Keyboard::Key key) {
            handleKeyPress(key);
            });

        initializeTileTypes();
        initializeWorldMap(mapWidth, mapHeight);

        try {
            loadMapFromFile("map.txt");
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading map: " << e.what() << std::endl;
            createDefaultGrid();
        }

        createPlayer();
        createCamera();
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Escape) {
            window.getWindow().close();
        }
    }

    void createCamera() {
        auto player = entityManager.getEntityByTag("Player");
        if (!player) {
            std::cerr << "Cannot create camera: Player not found!" << std::endl;
            return;
        }

        auto camera = std::make_shared<CameraComponent>();
        camera->offsetX = 0.0f;
        camera->offsetY = -100.0f;
        camera->zoom = 1.0f;
        camera->followPlayer = true;
        player->addComponent(camera);

        std::cout << "Camera created and attached to player" << std::endl;
    }

    void initializeTileTypes() {
        TileComponent grass;
        grass.tileId = 0;
        grass.isWalkable = true;
        grass.textureColumn = 0;
        grass.textureRow = 5;
        grass.name = "Grass";
        tileTypes[0] = grass;

        TileComponent dirt;
        dirt.tileId = 1;
        dirt.isWalkable = false;
        dirt.textureColumn = 1;
        dirt.textureRow = 2;
        dirt.name = "Dirt";
        tileTypes[1] = dirt;

       
        TileComponent water;
        water.tileId = 2;
        water.isWalkable = false;
        water.textureColumn = 2;
        water.textureRow = 0;
        water.name = "Water";
        tileTypes[2] = water;

        TileComponent stone;
        stone.tileId = 3;
        stone.isWalkable = true;
        stone.textureColumn = 3;
        stone.textureRow = 1;
        stone.name = "Stone";
        tileTypes[3] = stone;

        TileComponent sand;
        sand.tileId = 4;
        sand.isWalkable = true;
        sand.textureColumn = 4;
        sand.textureRow = 3;
        sand.name = "Sand";
        tileTypes[4] = sand;

        TileComponent wood;
        wood.tileId = 5;
        wood.isWalkable = true;
        wood.textureColumn = 5;
        wood.textureRow = 4;
        wood.name = "Wood";
        tileTypes[5] = wood;
    }

    TileComponent getTileTypeById(int id) {
        auto it = tileTypes.find(id);
        if (it != tileTypes.end()) {
            return it->second;
        }

        return tileTypes[0]; // Default to grass if not found
    }

    void initializeWorldMap(int width, int height) {
        mapWidth = width;
        mapHeight = height;

        worldMap.clear();

        for (int y = 0; y < height; y++) {
            std::vector<int> row;

            for (int x = 0; x < width; x++) {
                row.push_back(0); // Initialize with grass
            }

            worldMap.push_back(row);
        }
    }

    sf::Vector2f gridToWorld(int gridX, int gridY) {
        float worldX = (gridX - gridY) * (TILE_WIDTH / 2);
        float worldY = (gridX + gridY) * (TILE_HEIGHT / 2);

        worldX += GRID_OFFSET_X;
        worldY += GRID_OFFSET_Y;

        return sf::Vector2f(worldX, worldY);
    }

    void setMapTile(int x, int y, int tileType) {
        if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
            worldMap[y][x] = tileType;
        }
    }

    int getMapTile(int x, int y) {
        if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
            return worldMap[y][x];
        }
        return 0; // Default to grass
    }

    void createDefaultGrid() {
        clearTiles();

        initializeWorldMap(20, 20);
        //Note: Tileset changed!!!
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                if (x == 0 || y == 0 || x == 19 || y == 19) {
                    setMapTile(x, y, 2); // Water border
                }
                else if ((x > 3 && x < 7) && (y > 3 && y < 7)) {
                    setMapTile(x, y, 4); // Sand area
                }
                else if (x == 10 || y == 10) {
                    setMapTile(x, y, 3); // Stone path
                }
                else if ((x > 12 && x < 16) && (y > 12 && y < 16)) {
                    setMapTile(x, y, 5); // Wood area
                }
                else {
                    setMapTile(x, y, 0); // Default grass
                }
                createTile(x, y, getMapTile(x, y));
            }
        }

        std::cout << "Created 20x20 grid with pattern" << std::endl;
    }

    void createTile(int gridX, int gridY, int tileTypeId = 0) {
        TileComponent tileType = getTileTypeById(tileTypeId);

        auto tile = entityManager.createEntity("Tile");

        // Add isometric grid component
        auto isoGrid = std::make_shared<IsometricGridComponent>();
        isoGrid->gridX = gridX;
        isoGrid->gridY = gridY;
        isoGrid->tileWidth = TILE_WIDTH;
        isoGrid->tileHeight = TILE_HEIGHT;
        tile->addComponent(isoGrid);

        sf::Vector2f worldPos = gridToWorld(gridX, gridY);

        // Add transform component
        auto transform = std::make_shared<TransformComponent>();
        transform->x = worldPos.x;
        transform->y = worldPos.y;
        tile->addComponent(transform);

        auto render = std::make_shared<RenderComponent>();
        render->sprite.setTexture(tilesTexture);

        const int tileWidth = 256;
        const int tileHeight = 256;

        render->sprite.setTextureRect(sf::IntRect(
            tileType.textureColumn * tileWidth,
            tileType.textureRow * tileHeight,
            tileWidth,
            tileHeight
        ));

        // Set origin to bottom center
        render->sprite.setOrigin(tileWidth / 2, tileHeight);

        render->layerOrder = 0;
        tile->addComponent(render);

        // Add tile component
        auto tileComp = std::make_shared<TileComponent>();
        tileComp->tileId = tileType.tileId;
        tileComp->isWalkable = tileType.isWalkable;
        tileComp->textureColumn = tileType.textureColumn;
        tileComp->textureRow = tileType.textureRow;
        tileComp->name = tileType.name;
        tile->addComponent(tileComp);
    }

    void createPlayer() {
        auto player = entityManager.createEntity("Player");

        // Add isometric grid component
        auto isoGrid = std::make_shared<IsometricGridComponent>();
        isoGrid->gridX = 10;
        isoGrid->gridY = 10;
        isoGrid->tileWidth = TILE_WIDTH;
        isoGrid->tileHeight = TILE_HEIGHT;
        player->addComponent(isoGrid);

        sf::Vector2f worldPos = gridToWorld(isoGrid->gridX, isoGrid->gridY);

        // Add transform component
        auto transform = std::make_shared<TransformComponent>();
        transform->x = worldPos.x;
        transform->y = worldPos.y;
        player->addComponent(transform);

        // Add render component
        auto render = std::make_shared<RenderComponent>();
        render->sprite.setTexture(playerTexture);

        const int frameWidth = playerTexture.getSize().x / 8;  // 8 columns in the sprite sheet
        const int frameHeight = playerTexture.getSize().y / 8; // 8 rows in the sprite sheet

        render->sprite.setTextureRect(sf::IntRect(0, 4 * frameHeight, frameWidth, frameHeight));

        render->sprite.setOrigin(frameWidth / 2, frameHeight);

        render->layerOrder = 1;
        player->addComponent(render);

        auto collision = std::make_shared<CollisionComponent>();
        collision->width = 128.0f;
        collision->height = 64.0f;
        player->addComponent(collision);

        std::cout << "Player created at grid position: " << isoGrid->gridX << ", " << isoGrid->gridY << std::endl;
    }

    void loadMapFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open map file: " << filename << std::endl;
            throw std::runtime_error("Could not open map file");
        }

        clearTiles();

        file >> mapWidth >> mapHeight;

        if (mapWidth <= 0 || mapWidth > 100 || mapHeight <= 0 || mapHeight > 100) {
            std::cerr << "Invalid map dimensions: " << mapWidth << "x" << mapHeight << std::endl;
            throw std::runtime_error("Invalid map dimensions");
        }

        initializeWorldMap(mapWidth, mapHeight);

        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                int tileType;
                if (!(file >> tileType)) {
                    std::cerr << "Error reading tile at position " << x << "," << y << std::endl;
                    throw std::runtime_error("Map file format error");
                }
                setMapTile(x, y, tileType);
                createTile(x, y, tileType);
            }
        }

        std::cout << "Map loaded successfully from " << filename << ": "
            << mapWidth << "x" << mapHeight << std::endl;
    }

    void clearTiles() {
        for (auto& entity : entityManager.getEntities()) {
            if (entity->hasComponent<TileComponent>()) {
                entityManager.deactivateEntity(entity);
            }
        }
    }

    void updateEntityWorldPosition(std::shared_ptr<Entity> entity) {
        auto isoGrid = entity->getComponent<IsometricGridComponent>();
        auto transform = entity->getComponent<TransformComponent>();

        if (isoGrid && transform) {
            sf::Vector2f worldPos = gridToWorld(isoGrid->gridX, isoGrid->gridY);
            transform->x = worldPos.x;
            transform->y = worldPos.y;
        }
    }

    bool isTileWalkable(int gridX, int gridY) {
        if (gridX < 0 || gridX >= mapWidth || gridY < 0 || gridY >= mapHeight) {
            return false; // Out of bound
        }

        int tileType = getMapTile(gridX, gridY);
        TileComponent tileComp = getTileTypeById(tileType);
        return tileComp.isWalkable;
    }

    void handleInput() {
        auto player = entityManager.getEntityByTag("Player");
        if (!player) return;

        auto isoGrid = player->getComponent<IsometricGridComponent>();
        if (!isoGrid) return;

        movementCooldown -= deltaTime;

        if (movementCooldown <= 0) {
            int prevX = isoGrid->gridX;
            int prevY = isoGrid->gridY;
            int newX = prevX;
            int newY = prevY;

            bool moved = false;
            auto render = player->getComponent<RenderComponent>();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                newY = prevY - 1; // Try to move up in grid

                if (render) {
                    const int frameWidth = playerTexture.getSize().x / 8;
                    const int frameHeight = playerTexture.getSize().y / 8;
                    render->sprite.setTextureRect(sf::IntRect(0, 6 * frameHeight, frameWidth, frameHeight));
                }
                moved = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                newY = prevY + 1; // Try to move down in grid

                if (render) {
                    const int frameWidth = playerTexture.getSize().x / 8;
                    const int frameHeight = playerTexture.getSize().y / 8;
                    render->sprite.setTextureRect(sf::IntRect(0, 2 * frameHeight, frameWidth, frameHeight));
                }
                moved = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                newX = prevX - 1; // Try to move left in grid

                if (render) {
                    const int frameWidth = playerTexture.getSize().x / 8;
                    const int frameHeight = playerTexture.getSize().y / 8;
                    render->sprite.setTextureRect(sf::IntRect(0, 4 * frameHeight, frameWidth, frameHeight));
                }
                moved = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                newX = prevX + 1; // Try to move right in grid

                if (render) {
                    const int frameWidth = playerTexture.getSize().x / 8;
                    const int frameHeight = playerTexture.getSize().y / 8;
                    render->sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
                }
                moved = true;
            }

            if (moved) {
                movementCooldown = MOVEMENT_DELAY;

                if (isTileWalkable(newX, newY)) {
                    isoGrid->gridX = newX;
                    isoGrid->gridY = newY;
               
                    updateEntityWorldPosition(player);

                    std::cout << "Player moved to grid position: "
                        << isoGrid->gridX << ", " << isoGrid->gridY << std::endl;
                }
                else {
                    std::cout << "Movement blocked - no walkable tile at "
                        << newX << ", " << newY << std::endl;
                }
            }
        }
    }

    void update() {
        // Update entity manager
        entityManager.update(deltaTime);
    }

    void updateDeltaTime() {
        auto currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
    }

    void render() {
        cameraSystem.update(window.getWindow(), entityManager.getEntities(), deltaTime);
        window.clear();

        // Render all entities
        renderSystem.update(window.getWindow(), entityManager.getEntities());

        window.display();
    }

    void run() {
        while (window.isOpen()) {
            updateDeltaTime();
            window.pollEvents();
            handleInput();
            update();
            render();
        }
    }
};

int main() {
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Game crashed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}