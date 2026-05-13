#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

#include "EntityManager.h"
#include "Components.h"
#include "GameWindow.h"
#include "RenderSystem.h"
#include "CameraSystem.h"
#include "BuildingSystem.h"
#include "UIManager.h"
#include "EntityFactory.h"
#include "PathfindingSystem.h"
class Game {
private:
    GameWindow window;
    CameraSystem cameraSystem;
    RenderSystem renderSystem;

    int currentWave = 0;
    float waveTimer = 5.0f;          
    float timeBetweenWaves = 15.0f;  
    int enemiesToSpawn = 0;         
    float spawnTimer = 0.0f;
    float timeBetweenSpawns = 1.0f; 

    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<EntityFactory> entityFactory;
    std::unique_ptr<BuildingSystem> buildingSystem;
    std::unique_ptr<UIManager> uiManager;

    float deltaTime;
    std::chrono::steady_clock::time_point lastFrameTime;

    // Textures
    sf::Texture playerTexture;
    sf::Texture mageTexture;
    sf::Texture tilesTexture;
    sf::Texture buildingsTexture;
    sf::Texture propsTexture;
    sf::Font gameFont;

    const float TILE_WIDTH = 256.0f;
    const float TILE_HEIGHT = 128.0f;
    const float GRID_OFFSET_X = 960.0f;
    const float GRID_OFFSET_Y = 400.0f;

    std::vector<std::vector<int>> worldMap;
    int mapWidth = 20;
    int mapHeight = 20;

public:
    Game() : window(1280, 720, "IsCS"),lastFrameTime(std::chrono::steady_clock::now()),cameraSystem(1920, 1080) {



        loadTextures();

        entityManager = std::make_unique<EntityManager>();
        entityFactory = std::make_unique<EntityFactory>(*entityManager);

        registerAllPrefabs();

        buildingSystem = std::make_unique<BuildingSystem>(*entityManager, *entityFactory, worldMap, mapWidth, mapHeight);
        uiManager = std::make_unique<UIManager>();

       
        window.setKeyPressCallback([this](sf::Keyboard::Key key) {
            if (key == sf::Keyboard::Escape) window.getWindow().close();
            });

        window.setMouseClickCallback([this](int x, int y) {
            if (uiManager->handleMouseClick(x, y, window.getWindow().getSize().y, *entityFactory)) return;

            int selectedId = uiManager->getSelectedBuildingId();
            if (selectedId != -1) {
                auto player = entityManager->getEntityByTag("Player");
                auto eco = player ? player->getComponent<EconomyComponent>() : nullptr;

               
                int buildingCost = entityFactory->getPrefab(selectedId).cost;

             
                if (eco && eco->gold >= buildingCost) {
                    sf::Vector2f worldPos = window.getWindow().mapPixelToCoords(sf::Vector2i(x, y));
                    sf::Vector2i gridPos = buildingSystem->worldToGrid(worldPos);

                 
                    if (buildingSystem->buildStructure(gridPos.x, gridPos.y, selectedId)) {
                        eco->gold -= buildingCost;
                        std::cout << "Kalan Altin: " << eco->gold << std::endl;
                        uiManager->deselectBuilding();
                    }
                }
                else {
                    std::cout << "Yetersiz Altin! Gerekli: " << buildingCost << " / Senin Paran: " << (eco ? eco->gold : 0) << std::endl;
                    uiManager->deselectBuilding();
                }
            }
            });

      
        initializeWorldMap(mapWidth, mapHeight);
        try {
            loadMapFromFile("map.txt");
        }
        catch (const std::exception& e) {
            mapWidth = 40;
            mapHeight = 40;
            initializeWorldMap(mapWidth, mapHeight);
            createDefaultGrid();
        }
        

        createPlayer();
        createCamera();
    }

    void loadTextures() {
        if (!playerTexture.loadFromFile("MiniKingMan.png")) {
            playerTexture.create(128, 256);
        }
        if (!tilesTexture.loadFromFile("tiles.png")) {
            tilesTexture.create(256, 256);
        }
        buildingsTexture.loadFromFile("buildings.png");
        propsTexture.loadFromFile("props.png");
        mageTexture.loadFromFile("MiniMage.png");
    }

    void registerAllPrefabs() {
        int tw = 256, th = 256;
        // ZEMİNLER (0-9 arası ID'ler)
        entityFactory->registerPrefab(0, "Grass", true, 1, 1, tilesTexture, 0 * tw, 5 * th, tw, th, tw / 2.0f, th, 0,50);
        entityFactory->registerPrefab(1, "Dirt", false, 1, 1, tilesTexture, 1 * tw, 2 * th, tw, th, tw / 2.0f, th, 0,50);
        entityFactory->registerPrefab(2, "Water", false, 1, 1, tilesTexture, 2 * tw, 0, tw, th, tw / 2.0f, th, 0,50);
        entityFactory->registerPrefab(3, "Stone", true, 1, 1, tilesTexture, 3 * tw, 1 * th, tw, th, tw / 2.0f, th, 0,50);
        entityFactory->registerPrefab(4, "Sand", true, 1, 1, tilesTexture, 4 * tw, 3 * th, tw, th, tw / 2.0f, th, 0,50);
        entityFactory->registerPrefab(5, "Wood", true, 1, 1, tilesTexture, 5 * tw, 4 * th, tw, th, tw / 2.0f, th, 0,50);
        entityFactory->registerPrefab(6, "Wall", false, 1, 1, tilesTexture, 6 * tw, 0, tw, th, tw / 2.0f, th, 1,50);

        // BİNALAR VE OBJELER (10 ve üzeri ID'ler)
        entityFactory->registerPrefab(10, "Tavern", false, 4, 4, buildingsTexture, 60, 50, 900, 900, 450.0f, 567.0f, 1,50);
        entityFactory->registerPrefab(11, "Wall1", false, 1, 1, propsTexture, 280, 500, 250, 300, 100.0f, 300.0f, 1,1); // wall
        entityFactory->registerPrefab(12, "Small Hut", false, 2, 2, buildingsTexture, 90, 1060, 950, 1000, 150.0f, 450.0f, 1,50);
        entityFactory->registerPrefab(13, "Well", false, 1, 1, buildingsTexture, 0, 300, 100, 150, 50.0f, 150.0f, 1,50);
        entityFactory->registerPrefab(14, "Rock", false, 1, 1, propsTexture, 100, 50, 80, 80, 40.0f, 80.0f, 1,50);
        entityFactory->registerPrefab(15, "Bush", false, 1, 1, propsTexture, 200, 50, 100, 80, 50.0f, 80.0f, 1,50);
        
        entityFactory->registerPrefab(20, "Turret", false, 1, 1, mageTexture, 10.0f, 17.0f, 13.0f, 13.0f, 5.0f, 7.0f, 1, 25);
    }

    void initializeWorldMap(int width, int height) {
        worldMap.assign(height, std::vector<int>(width, 0)); // 0 = Çim ile doldur
    }

    void createDefaultGrid() {
        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                int tileId = 0; // Varsayılan Çim
                if (x == 0 || y == 0 || x == mapWidth - 1 || y == mapHeight - 1) tileId = 2; // Su Kenarı
                else if (x == 10 || y == 10) tileId = 3; // Taş Yol

                worldMap[y][x] = tileId;
                entityFactory->createEntityAt(x, y, tileId);
            }
        }
    }
    void loadMapFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Harita dosyasi bulunamadi: " << filename << ", varsayilan harita yukleniyor." << std::endl;
            throw std::runtime_error("Could not open map file");
        }

        file >> mapWidth >> mapHeight;

        if (mapWidth <= 0 || mapWidth > 100 || mapHeight <= 0 || mapHeight > 100) {
            std::cerr << "Gecersiz harita boyutu: " << mapWidth << "x" << mapHeight << std::endl;
            throw std::runtime_error("Invalid map dimensions");
        }

        initializeWorldMap(mapWidth, mapHeight);

        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                int tileType;
                if (!(file >> tileType)) {
                    std::cerr << "Harita okunurken hata! Konum: " << x << "," << y << std::endl;
                    throw std::runtime_error("Map file format error");
                }

                worldMap[y][x] = tileType;
                entityFactory->createEntityAt(x, y, tileType);
            }
        }

        std::cout << "Harita basariyla yuklendi: " << filename << " (" << mapWidth << "x" << mapHeight << ")" << std::endl;
    }

    bool isTileWalkable(int gridX, int gridY) {
        if (gridX < 0 || gridX >= mapWidth || gridY < 0 || gridY >= mapHeight) return false;
        int tileType = worldMap[gridY][gridX];

 
        return entityFactory->getPrefab(tileType).isWalkable;
    }

    void createPlayer() {
        auto player = entityManager->createEntity("Player");
        auto health = std::make_shared<HealthComponent>();
        player->addComponent(health);
        auto eco = std::make_shared<EconomyComponent>();
        eco->gold = 100;
        player->addComponent(eco);

        auto stamina = std::make_shared<StaminaComponent>();
        player->addComponent(stamina);
        //ANİMASYON AYARLARI
        auto anim = std::make_shared<AnimationComponent>();
        anim->currentRow = 0;   // 0. Satır (Bekleme)
        anim->totalFrames = 4;  // Bekleme animasyonu 4 kare

       
        anim->frameWidth = 32;
        anim->frameHeight = 32;
        player->addComponent(anim);

        auto move = std::make_shared<MovementComponent>();
        player->addComponent(move);

        auto isoGrid = std::make_shared<IsometricGridComponent>();
        isoGrid->gridX = 10;
        isoGrid->gridY = 10;
        player->addComponent(isoGrid);

        auto transform = std::make_shared<TransformComponent>();
        sf::Vector2f worldPos = entityFactory->gridToWorld(10, 10);
        transform->x = worldPos.x;
        transform->y = worldPos.y;
        player->addComponent(transform);

        // RENDER AYARLARI
        auto render = std::make_shared<RenderComponent>();
        render->sprite.setTexture(playerTexture);

        // Makas
        render->sprite.setTextureRect(sf::IntRect(0, 0, anim->frameWidth, anim->frameHeight));
        render->sprite.setOrigin(anim->frameWidth / 2.0f, anim->frameHeight);

        transform->scaleX = 16.0f;
        transform->scaleY = 16.0f;

        render->layerOrder = 1;
        player->addComponent(render);
    }
    void updateWaves() {

        if (enemiesToSpawn > 0) {
            spawnTimer += deltaTime;
            if (spawnTimer >= timeBetweenSpawns) {
                spawnTimer = 0.0f;
                enemiesToSpawn--;

                int spawnX, spawnY;
                do {
                 
                    spawnX = (rand() % 2 == 0) ? 0 : mapWidth - 1;
                    spawnY = rand() % mapHeight;
                } while (!isTileWalkable(spawnX, spawnY)); 

                createEnemy(spawnX, spawnY);
            }
        }
   
        else {
            waveTimer += deltaTime;
            if (waveTimer >= timeBetweenWaves) {
                waveTimer = 0.0f;
                currentWave++;

                enemiesToSpawn = currentWave * 3;

                std::cout << "--- DALGA " << currentWave << " BASLADI! ("
                    << enemiesToSpawn << " Dusman Geliyor!) ---" << std::endl;
            }
        }
    }

    bool isGridOccupied(int targetX, int targetY, std::shared_ptr<Entity> mySelf) {
        for (auto& entity : entityManager->getEntities()) {
           
            if (entity == mySelf) continue;

            auto isoGrid = entity->getComponent<IsometricGridComponent>();
            auto move = entity->getComponent<MovementComponent>(); 

            if (isoGrid && move) {
                
                if (isoGrid->gridX == targetX && isoGrid->gridY == targetY) {
                    return true;
                }
            }
        }
        return false;
    }
    void applySeparation() {
        auto enemies = entityManager->getEntitiesByTag("Enemy");

        // AYARLAR
        float separationRadius = 40.0f; 
        float pushStrength = 300.0f;    

     
        for (size_t i = 0; i < enemies.size(); ++i) {
            auto t1 = enemies[i]->getComponent<TransformComponent>();
            if (!t1) continue;

        
            for (size_t j = i + 1; j < enemies.size(); ++j) {
                auto t2 = enemies[j]->getComponent<TransformComponent>();
                if (!t2) continue;

              
                float dx = t1->x - t2->x;
                float dy = t1->y - t2->y;
                float dist = std::sqrt(dx * dx + dy * dy);

              
                if (dist == 0.0f) {
                    dx = ((rand() % 100) - 50) * 0.01f;
                    dy = ((rand() % 100) - 50) * 0.01f;
                    dist = std::sqrt(dx * dx + dy * dy);
                }

            
                if (dist < separationRadius) {
                
                    float nx = dx / dist;
                    float ny = dy / dist;

               
                    float overlap = (separationRadius - dist) / separationRadius;

                   
                    float pushX = nx * overlap * pushStrength * deltaTime;
                    float pushY = ny * overlap * pushStrength * deltaTime;

                   
                    t1->x += pushX;
                    t1->y += pushY;

             
                    t2->x -= pushX;
                    t2->y -= pushY;
                }
            }
        }
    }
    void createEnemy(int startX, int startY) {
        auto enemy = entityManager->createEntity("Enemy"); 
        auto health = std::make_shared<HealthComponent>();
        health->maxHealth = 100.0f;
        health->currentHealth = 100.0f;
        enemy->addComponent(health);
      
        auto anim = std::make_shared<AnimationComponent>();
        anim->currentRow = 0;
        anim->totalFrames = 4;
        anim->frameWidth = 32;  
        anim->frameHeight = 32;
        enemy->addComponent(anim);

        
        auto move = std::make_shared<MovementComponent>();
        enemy->addComponent(move);

        auto ai = std::make_shared<AIComponent>();
        ai->thinkInterval = 1.0f; 
        enemy->addComponent(ai);

        
        auto isoGrid = std::make_shared<IsometricGridComponent>();
        isoGrid->gridX = startX;
        isoGrid->gridY = startY;
        enemy->addComponent(isoGrid);

        auto transform = std::make_shared<TransformComponent>();
        sf::Vector2f worldPos = entityFactory->gridToWorld(startX, startY);
        transform->x = worldPos.x;
        transform->y = worldPos.y;
        transform->scaleX = 16.0f; 
        transform->scaleY = 16.0f;
        enemy->addComponent(transform);

        //Düşman Resim
        auto render = std::make_shared<RenderComponent>();
        render->sprite.setTexture(playerTexture);
        render->sprite.setTextureRect(sf::IntRect(0, 0, anim->frameWidth, anim->frameHeight));
        render->sprite.setOrigin(anim->frameWidth / 2.0f, anim->frameHeight);

       
        render->sprite.setColor(sf::Color(255, 100, 100));

        render->layerOrder = 1;
        enemy->addComponent(render);
    }
    void createCamera() {
        auto player = entityManager->getEntityByTag("Player");
        if (player) {
            auto camera = std::make_shared<CameraComponent>();
            camera->offsetX = 0.0f;
            camera->offsetY = -100.0f;
            camera->followPlayer = true;
            player->addComponent(camera);
        }
    }
    void updateDecay() {
        for (auto& entity : entityManager->getEntities()) {
            auto health = entity->getComponent<HealthComponent>();
            if (health && health->currentHealth <= 0) {
                health->deathTimer += deltaTime;

                auto render = entity->getComponent<RenderComponent>();
                if (render && health->deathTimer > (health->decayTime - 2.0f)) {
                   
                    float alpha = 255 * (1.0f - (health->deathTimer - (health->decayTime - 2.0f)) / 2.0f);
                    if (alpha < 0) alpha = 0;

                    sf::Color color = render->sprite.getColor();
                    render->sprite.setColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(alpha)));
                }

                if (health->deathTimer >= health->decayTime) {
                    entityManager->deactivateEntity(entity);
                }
            }
        }
    }
    void handleInput() {
        auto player = entityManager->getEntityByTag("Player");
        if (!player) return;

        auto isoGrid = player->getComponent<IsometricGridComponent>();
        auto anim = player->getComponent<AnimationComponent>();
        auto move = player->getComponent<MovementComponent>();
        auto transform = player->getComponent<TransformComponent>();

        if (!isoGrid || !anim || !move || !transform) return;

   
        if (move->isAttacking) return;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

                move->isAttacking = true;
                move->path.clear();
                move->currentPathIndex = 0;
                anim->currentRow = 4; // Saldırı animasyonu
                anim->totalFrames = 10;
                anim->currentFrame = 0;
                anim->frameDuration = 0.08f;
                anim->isPlaying = true;

         
                float totalAttackDamage = 75.0f; 
                std::vector<std::shared_ptr<Entity>> targetsInMelee; 

             
                for (auto& enemy : entityManager->getEntitiesByTag("Enemy")) {
                    auto enemyHealth = enemy->getComponent<HealthComponent>();
                    auto enemyGrid = enemy->getComponent<IsometricGridComponent>();

                    
                    if (enemyHealth && enemyHealth->currentHealth > 0 && enemyGrid) {

                       
                        int dx = std::abs(enemyGrid->gridX - isoGrid->gridX);
                        int dy = std::abs(enemyGrid->gridY - isoGrid->gridY);

                      
                        if (dx <= 1 && dy <= 1) {
                            targetsInMelee.push_back(enemy);
                        }
                    }
                }

               
                if (!targetsInMelee.empty()) {
                    float damagePerEnemy = totalAttackDamage / targetsInMelee.size();
                    std::cout << targetsInMelee.size() << " dusmana " << damagePerEnemy << " hasar vuruldu!" << std::endl;

                    for (auto& enemy : targetsInMelee) {
                        auto enemyHealth = enemy->getComponent<HealthComponent>();
                        enemyHealth->currentHealth -= damagePerEnemy;

                   
                        if (enemyHealth->currentHealth <= 0) {
                            enemyHealth->currentHealth = 0;

                           
                            auto eco = player->getComponent<EconomyComponent>();
                            if (eco) {
                                eco->gold += 15; 
                                std::cout << "+15 Altin Kazandin! Toplam: " << eco->gold << std::endl;
                            }

                            auto enemyAnim = enemy->getComponent<AnimationComponent>();
                            auto enemyMove = enemy->getComponent<MovementComponent>();

                            if (enemyAnim && enemyMove) {
                           
                                enemyMove->path.clear();
                                enemyMove->isMoving = false;

                                enemyAnim->currentRow = 6; // Ölüm animasyonu
                                enemyAnim->totalFrames = 4;
                                enemyAnim->currentFrame = 0;
                                enemyAnim->frameDuration = 0.2f; 
                                enemyAnim->isPlaying = true;
                            }
                        }
                    }
                }
            
            return;
        }

        // SAĞ TIK İLE A* YOLUNU BUL
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window.getWindow());
            sf::Vector2f worldPos = window.getWindow().mapPixelToCoords(mousePos);
            sf::Vector2i gridPos = buildingSystem->worldToGrid(worldPos);

            
            move->path = PathfindingSystem::findPath(
                sf::Vector2i(isoGrid->gridX, isoGrid->gridY),
                gridPos,
                [this](int x, int y) { return isTileWalkable(x, y); }
            );
            move->currentPathIndex = 0; 
        }
    }

    void updateMovement() {
        auto player = entityManager->getEntityByTag("Player");
        for (auto& entity : entityManager->getEntities()) {
            auto health = entity->getComponent<HealthComponent>();
            if (health && health->currentHealth <= 0) continue; 
           
            auto move = entity->getComponent<MovementComponent>();
            auto transform = entity->getComponent<TransformComponent>();
            auto anim = entity->getComponent<AnimationComponent>();
            auto isoGrid = entity->getComponent<IsometricGridComponent>();

            if (!move || !transform || !anim || !isoGrid) continue;

     
            if (move->isMoving) {
                move->moveTimer += deltaTime;
                float t = move->moveTimer / move->moveDuration;
                if (t >= 1.0f) {
                    t = 1.0f;
                    move->isMoving = false; 
                    move->currentPathIndex++; 
                }
                transform->x = move->startWorldPos.x + (move->targetWorldPos.x - move->startWorldPos.x) * t;
                transform->y = move->startWorldPos.y + (move->targetWorldPos.y - move->startWorldPos.y) * t;
            }
           
            else if (move->currentPathIndex < move->path.size() && !move->isAttacking) {
                sf::Vector2i nextStep = move->path[move->currentPathIndex];

                if (entity != player && isGridOccupied(nextStep.x, nextStep.y, entity)) {
                    anim->isPlaying = false;
                    anim->currentRow = 0; // (Idle)
                    anim->totalFrames = 4;
                    anim->currentFrame = 0; 
                    continue; 
                }

               
                anim->currentRow = 4;

                if (!move->isMoving) {
                    anim->currentFrame = 0; 
                }

                anim->totalFrames = 10;
                anim->frameDuration = 0.1f;
                anim->isPlaying = true;

                move->isMoving = true;
                move->moveTimer = 0.0f;
                move->startWorldPos = sf::Vector2f(transform->x, transform->y);
                move->targetWorldPos = entityFactory->gridToWorld(nextStep.x, nextStep.y);

               
                isoGrid->gridX = nextStep.x;
                isoGrid->gridY = nextStep.y;
            }
 
            else if (!move->isAttacking) {
                anim->isPlaying = false;
                anim->currentRow = 0;
                anim->totalFrames = 4;
            }
        }
    }
    void updateTavern() {
        auto player = entityManager->getEntityByTag("Player");
        if (!player) return;

        auto ecoComp = player->getComponent<EconomyComponent>();
        if (!ecoComp) return;

       
        for (auto& tavern : entityManager->getEntitiesByTag("Tavern")) {
            auto prod = tavern->getComponent<ProductionComponent>();

            if (prod) {
                prod->timer += deltaTime;

                if (prod->timer >= prod->interval) {
                    ecoComp->gold += prod->amount;
                    prod->timer = 0.0f; 
                    std::cout << "Kadar Altın Eklendi:  " << ecoComp->gold << std::endl;
                }
            }
        }
    }
    void updateTurrets() {
    
        for (auto& turret : entityManager->getEntitiesByTag("Turret")) {
            auto tComp = turret->getComponent<TurretComponent>();
            auto tTrans = turret->getComponent<TransformComponent>();
            if (!tComp || !tTrans) continue;

            tTrans->scaleX = 16.0f;
            tTrans->scaleY = 16.0f;
            tComp->fireTimer += deltaTime;

          
            if (tComp->fireTimer >= tComp->fireRate) {

                std::shared_ptr<Entity> closestEnemy = nullptr;
                float minDst = tComp->attackRange;

               
                for (auto& enemy : entityManager->getEntitiesByTag("Enemy")) {
                    auto eHealth = enemy->getComponent<HealthComponent>();
                    if (eHealth && eHealth->currentHealth <= 0) continue; 

                    auto eTrans = enemy->getComponent<TransformComponent>();
                    if (eTrans) {
                        float dx = eTrans->x - tTrans->x;
                        float dy = eTrans->y - tTrans->y;
                        float dst = std::sqrt(dx * dx + dy * dy);

                        if (dst < minDst) {
                            minDst = dst;
                            closestEnemy = enemy;
                        }
                    }
                }

                
                if (closestEnemy) {
                    tComp->fireTimer = 0.0f;  
                    spawnProjectile(tTrans->x, tTrans->y - 80.0f, closestEnemy, tComp->damage);
                }
            }
        }
    }

    void updateProjectiles() {
        for (auto& proj : entityManager->getEntitiesByTag("Projectile")) {
            auto pComp = proj->getComponent<ProjectileComponent>();
            auto pTrans = proj->getComponent<TransformComponent>();
            if (!pComp || !pTrans) continue;

            // Eğer merminin hedefi yolda öldüyse (başka kule vurduysa vs.), mermiyi yok et
            auto tHealth = pComp->target ? pComp->target->getComponent<HealthComponent>() : nullptr;
            if (!pComp->target || !pComp->target->isActive() || (tHealth && tHealth->currentHealth <= 0)) {
                entityManager->deactivateEntity(proj);
                continue;
            }

            auto tTrans = pComp->target->getComponent<TransformComponent>();

            // Hedefe olan mesafeyi ölç (Hedefin kafasına/göğsüne gitmesi için Y'yi biraz eksilttik)
            float dx = tTrans->x - pTrans->x;
            float dy = (tTrans->y - 32.0f) - pTrans->y;
            float dist = std::sqrt(dx * dx + dy * dy);

        
            if (dist < 10.0f) {
                tHealth->currentHealth -= pComp->damage;

                
                if (tHealth->currentHealth <= 0) {
                    tHealth->currentHealth = 0;

                   
                    auto player = entityManager->getEntityByTag("Player");
                    auto eco = player ? player->getComponent<EconomyComponent>() : nullptr;
                    if (eco) eco->gold += 15;

                    auto anim = pComp->target->getComponent<AnimationComponent>();
                    auto move = pComp->target->getComponent<MovementComponent>();
                    if (anim && move) {
                        move->isMoving = false;
                        move->path.clear();
                        anim->currentRow = 6;
                        anim->totalFrames = 4;
                        anim->currentFrame = 0;
                        anim->isPlaying = true;
                    }
                }
                entityManager->deactivateEntity(proj); 
            }
       
            else {
                float nx = dx / dist;
                float ny = dy / dist;
                pTrans->x += nx * pComp->speed * deltaTime;
                pTrans->y += ny * pComp->speed * deltaTime;
            }
        }
    }
    void spawnProjectile(float startX, float startY, std::shared_ptr<Entity> target, int damage) {
        auto proj = entityManager->createEntity("Projectile");

        auto trans = std::make_shared<TransformComponent>();
        trans->x = startX;
        trans->y = startY;
        proj->addComponent(trans);

        auto render = std::make_shared<RenderComponent>();
        
        render->sprite.setTexture(propsTexture);
        render->sprite.setTextureRect(sf::IntRect(100, 50, 150, 100));
        render->sprite.setOrigin(7.5f, 7.5f);
        render->sprite.setColor(sf::Color(255, 200, 0)); 
        render->layerOrder = 3; 
        proj->addComponent(render);

        auto pComp = std::make_shared<ProjectileComponent>();
        pComp->target = target;
        pComp->damage = damage;
        proj->addComponent(pComp);
    }
    void updateEnemies() {
        auto player = entityManager->getEntityByTag("Player");
        if (!player) return;

        auto playerGrid = player->getComponent<IsometricGridComponent>();
        auto playerHealth = player->getComponent<HealthComponent>();

        for (auto& entity : entityManager->getEntitiesByTag("Enemy")) {
           
            auto health = entity->getComponent<HealthComponent>();
            if (health && health->currentHealth <= 0) continue;

            auto ai = entity->getComponent<AIComponent>();
            auto move = entity->getComponent<MovementComponent>();
            auto enemyGrid = entity->getComponent<IsometricGridComponent>();
            auto anim = entity->getComponent<AnimationComponent>();

            if (ai && move && enemyGrid && playerGrid) {
                int dx = std::abs(enemyGrid->gridX - playerGrid->gridX);
                int dy = std::abs(enemyGrid->gridY - playerGrid->gridY);

                if (dx <= 1 && dy <= 1) {
                    
                    if (!move->isAttacking) {
                        move->path.clear();
                    }

                    ai->attackTimer += deltaTime;
                    if (ai->attackTimer >= ai->attackRate) {
                        ai->attackTimer = 0.0f; 

                        if (playerHealth && playerHealth->currentHealth > 0) {
                            playerHealth->currentHealth -= ai->attackDamage;
                            std::cout << "Dusman Vurdu! Kalan Canin: " << playerHealth->currentHealth << std::endl;

                            move->isAttacking = true;
                            anim->currentRow = 4;
                            anim->totalFrames = 10;
                            anim->currentFrame = 0;
                            anim->frameDuration = 0.08f;
                            anim->isPlaying = true;

                            if (playerHealth->currentHealth <= 0) {
                                playerHealth->currentHealth = 0;
                                std::cout << "--- GAME OVER! KAHRAMAN DUSTU! ---" << std::endl;

                                auto pAnim = player->getComponent<AnimationComponent>();
                                auto pMove = player->getComponent<MovementComponent>();
                                if (pAnim && pMove) {
                                    pMove->isAttacking = false;
                                    pMove->path.clear();
                                    pMove->isMoving = false;

                                    pAnim->currentRow = 6; 
                                    pAnim->totalFrames = 4;
                                    pAnim->currentFrame = 0;
                                    pAnim->isPlaying = true;
                                }
                            }
                        }
                    }
                }
                else {
                    ai->attackTimer = ai->attackRate;

                    ai->thinkTimer += deltaTime;
                    if (ai->thinkTimer >= ai->thinkInterval) {
                        ai->thinkTimer = 0.0f;

                        move->path = PathfindingSystem::findPath(
                            sf::Vector2i(enemyGrid->gridX, enemyGrid->gridY),
                            sf::Vector2i(playerGrid->gridX, playerGrid->gridY),
                            [this](int x, int y) { return isTileWalkable(x, y); }
                        );
                        move->currentPathIndex = 0;
                    }
                }
            }
        }
    }
    void setAnimation(std::shared_ptr<Entity> entity, int row, int frames, float speed = 0.1f) {
        auto anim = entity->getComponent<AnimationComponent>();
        if (anim && anim->currentRow != row) { 
            anim->currentRow = row;
            anim->totalFrames = frames;
            anim->currentFrame = 0; 
            anim->frameDuration = speed;
            anim->isPlaying = true;
        }
    }
    void updateAnimations() {
        for (auto& entity : entityManager->getEntities()) {
            auto anim = entity->getComponent<AnimationComponent>();
            auto render = entity->getComponent<RenderComponent>();
            auto move = entity->getComponent<MovementComponent>(); 

            if (anim && render) {
                if (anim->isPlaying) {
                    anim->elapsedTime += deltaTime;
                    if (anim->elapsedTime >= anim->frameDuration) {
                        anim->elapsedTime = 0.0f;
                        anim->currentFrame++;
                        auto health = entity->getComponent<HealthComponent>();
                        if (health && health->currentHealth <= 0 && anim->currentRow == 6) {
                            if (anim->currentFrame >= anim->totalFrames) {
                                anim->currentFrame = anim->totalFrames - 1; 
                                anim->isPlaying = false; 
                            }
                        }
                       
                        if (move && move->isAttacking && anim->currentFrame >= anim->totalFrames) {
                            move->isAttacking = false;
                            anim->currentRow = 0; // Bekleme animasyonuna dön
                            anim->totalFrames = 4;
                            anim->currentFrame = 0;
                            anim->isPlaying = false; 
                        }
                        else {
                            
                            anim->currentFrame = anim->currentFrame % anim->totalFrames;
                        }

                        render->sprite.setTextureRect(sf::IntRect(
                            anim->currentFrame * anim->frameWidth,
                            anim->currentRow * anim->frameHeight,
                            anim->frameWidth,
                            anim->frameHeight
                        ));
                    }
                }
                else {
                    anim->currentFrame = 0;
                    anim->elapsedTime = 0.0f;
                    render->sprite.setTextureRect(sf::IntRect(
                        0,
                        anim->currentRow * anim->frameHeight,
                        anim->frameWidth,
                        anim->frameHeight
                    ));
                }
            }
        }
    }

    void update() {
        auto currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        entityManager->update(deltaTime);
        updateDecay();
        updateMovement();
        applySeparation();
        updateTurrets();
        updateTavern();
        updateProjectiles();
        updateWaves();
        updateEnemies();
        updateAnimations();
    }

    void render() {
        window.clear();

        cameraSystem.update(window.getWindow(), entityManager->getEntities(), deltaTime);
        renderSystem.update(window.getWindow(), entityManager->getEntities());


        uiManager->drawGhostBuilding(window.getWindow(), *buildingSystem, *entityFactory);
     
        sf::View currentCameraView = window.getWindow().getView();

       
        window.getWindow().setView(window.getWindow().getDefaultView());

       
        uiManager->drawUI(window.getWindow(), *entityFactory);

        
        window.getWindow().setView(currentCameraView);

        window.display();
    }
    void run() {
        while (window.isOpen()) {
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