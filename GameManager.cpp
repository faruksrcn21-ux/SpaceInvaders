#include "GameManager.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstdio>
#include "Enemy.h"

GameManager::GameManager() : window(sf::VideoMode(800, 600), "Space Invaders") {
    srand(time(0));


    // Font yükleme (üst dizine ../assets olarak da bak)
    bool fontLoaded = false;
    fontLoaded = font.loadFromFile("C:/Windows/Fonts/consola.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("../assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("../../assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/courier.ttf");
    if (!fontLoaded)
        printf("[UYARI] Font yuklenemedi!\n"
               "        Cozum: assets/font.ttf'i proje klasorune kopyalayin.\n");


    // HUD metinleri
    scoreText.setFont(font);  scoreText.setCharacterSize(20);  scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, 10.f);  scoreText.setString("Skor: 0");
 
    livesText.setFont(font);  livesText.setCharacterSize(20);  livesText.setFillColor(sf::Color::White);
    livesText.setPosition(650.f, 10.f); livesText.setString("Can: 3");
 
    levelText.setFont(font);  levelText.setCharacterSize(20);  levelText.setFillColor(sf::Color::White);
    levelText.setPosition(350.f, 10.f); levelText.setString("Seviye: 1");


    // Game Over metni
    gameOverText.setFont(font); gameOverText.setCharacterSize(80); gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");
    sf::FloatRect r = gameOverText.getLocalBounds();
    gameOverText.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    gameOverText.setPosition(400.f, 260.f);
    

    // Menü metinleri
    menuTitleText.setFont(font);
    menuSubText.setFont(font);
    winText.setFont(font);
    winSubText.setFont(font);
    restartHintText.setFont(font);
    
    
    // Bariyerler
    barriers.push_back(Barrier(100.0f, 450.0f));
    barriers.push_back(Barrier(350.0f, 450.0f));
    barriers.push_back(Barrier(600.0f, 450.0f));
    

    // Formasyon
    swarmSpeed         = 100.0f;
    swarmDirection     = 1;
    dropDistance       = 20.0f;
    enemyShootTimer    = 0.0f;
    enemyShootInterval = 1.0f;
    dropPending        = false;
    swarmMoveTimer     = 0.0f;
    swarmMoveInterval  = 0.8f;
    

    // Dokunulmazlık başlangıç değerleri
    invincibleTimer = 0.0f;
    blinkTimer      = 0.0f;
    playerVisible   = true;


    // Oyun durumunu başlat
    gameState  = State::Playing;
    score      = 0;
    lives      = 3;
    level      = 1;
    isGameOver = false;

    initLevel();
}

void GameManager::initLevel() {
    enemies.clear();
    int rows = 4; int cols = 8;
    float startX = 50.0f; float startY = 50.0f;
    float spacingX = 60.0f; float spacingY = 50.0f;
 
    //   Her satıra farklı tip atanır
    //   Sıra 0 (en üst)  → Tip A — Pembe   — 30 puan
    //   Sıra 1-2         → Tip B — Turuncu  — 20 puan
    //   Sıra 3 (en alt)  → Tip C — Kırmızı — 10 puan
    for (int i = 0; i < rows; i++) {
        EnemyType type;
        if      (i == 0) type = EnemyType::A;
        else if (i <= 2) type = EnemyType::B;
        else             type = EnemyType::C;
 
        for (int j = 0; j < cols; j++) {
            enemies.push_back(
                Enemy(startX + j * spacingX, startY + i * spacingY, type)
            );
        }
    }
}

void GameManager::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.05f) deltaTime = 0.05f;
        processEvents();
        update(deltaTime);
        render();
    }
}

void GameManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape) window.close();
    }
}

void GameManager::update(float DeltaTime) {
    if (gameState != State::Playing) return;
 
    player.update(DeltaTime, bullets);
 
    // Dokunulmazlık zamanlayıcısı + yanıp sönme

    //   invincibleTimer > 0 iken düşman mermisi oyuncuya zarar vermez.
    //   Vurulunca INV_DURATION (2 sn) dokunulmazlık başlar.
    //   Bu süre zarfında oyuncu BLINK_RATE (0.12 sn) aralıklarla
    //   görünür/görünmez olur — klasik arcade yanıp sönme efekti.
    
    if (invincibleTimer > 0.0f) {
        invincibleTimer -= DeltaTime;
 
        // Yanıp sönme: blinkTimer BLINK_RATE'e ulaşınca playerVisible toggle
        blinkTimer += DeltaTime;
        if (blinkTimer >= BLINK_RATE) {
            blinkTimer    = 0.0f;
            playerVisible = !playerVisible;
        }
 
        if (invincibleTimer <= 0.0f) {
            invincibleTimer = 0.0f;
            playerVisible   = true;   // dokunulmazlık bitince görünür kal
        }
    }
      // Oyuncu mermileri
    std::vector<bool> bulletAlive(bullets.size(), true);
    for (int i = 0; i < (int)bullets.size(); i++) {
        bullets[i].update(DeltaTime);
        if (bullets[i].getY() < 0) { bulletAlive[i] = false; continue; }
 
        bool hit = false;
        for (auto& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            if (bullets[i].getBounds().intersects(enemy.getBounds())) {
                enemy.takeDamage(1);
                score += enemy.getScore();
                scoreText.setString("Skor: " + std::to_string(score));
                hit = true; break;
            }
        }
        if (hit) { bulletAlive[i] = false; continue; }
 
        for (auto& barrier : barriers) {
            auto& blocks = barrier.getBlocks();
            for (int k = 0; k < (int)blocks.size(); k++) {
                if (bullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                    blocks.erase(blocks.begin() + k);
                    bulletAlive[i] = false; hit = true; break;
                }
            }
            if (hit) break;
        }
    }
    { int idx = 0; bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [&](const Bullet&){ return !bulletAlive[idx++]; }), bullets.end()); }

        // Formasyon hareketi
    swarmMoveTimer += DeltaTime;
    if (swarmMoveTimer >= swarmMoveInterval) {
        swarmMoveTimer = 0.0f;
        if (dropPending) {
            for (auto& e : enemies) e.move(0.f, dropDistance);
            swarmDirection *= -1; dropPending = false;
        } else {
            float step = swarmSpeed * swarmDirection * swarmMoveInterval;
            bool hitWall = false;
            for (auto& e : enemies) {
                float nx = e.getX() + step;
                if (nx <= 0.f || nx >= 760.f) { hitWall = true; break; }
            }
            if (hitWall) dropPending = true;
            else for (auto& e : enemies) e.move(step, 0.f);
 
            float ratio = 1.f - (float)enemies.size() / (4.f * 8.f);
            swarmMoveInterval = 0.8f * (1.f - ratio * 0.75f);
            if (swarmMoveInterval < 0.05f) swarmMoveInterval = 0.05f;
        }
    }
 
    // Düşman ateşi
    enemyShootTimer += DeltaTime;
    if (enemyShootTimer >= enemyShootInterval && !enemies.empty()) {
        int idx = rand() % enemies.size();
        enemyBullets.push_back(Bullet(enemies[idx].getX() + 20.f,
                                      enemies[idx].getY() + 30.f, 1.f, sf::Color::Red));
        enemyShootTimer    = 0.f;
        enemyShootInterval = 0.5f + (float)rand() / (float)(RAND_MAX / 1.5f);
    }

    // Ölü düşmanlar
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e){ return !e.isAlive(); }), enemies.end());
 
    // Düşman mermileri
    std::vector<bool> eBulletAlive(enemyBullets.size(), true);
    for (int i = 0; i < (int)enemyBullets.size(); i++) {
        enemyBullets[i].update(DeltaTime);
        if (enemyBullets[i].getY() > 600) { eBulletAlive[i] = false; continue; }

        // Sadece dokunulmazlık yoksa hasar ver
        if (invincibleTimer <= 0.0f &&
            enemyBullets[i].getBounds().intersects(player.getBounds()))
        {
            lives--;
            livesText.setString("Can: " + std::to_string(lives));
            eBulletAlive[i] = false;
 
            if (lives <= 0) {
                isGameOver = true;
                gameState  = State::GameOver;
            } else {
                // Vurulunca dokunulmazlık başlat
                invincibleTimer = INV_DURATION;
                blinkTimer      = 0.0f;
                playerVisible   = true;
            }
            continue;
        }

        bool hit = false;
        for (auto& barrier : barriers) {
            auto& blocks = barrier.getBlocks();
            for (int k = 0; k < (int)blocks.size(); k++) {
                if (enemyBullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                    blocks.erase(blocks.begin() + k);
                    eBulletAlive[i] = false; hit = true; break;
                }
            }
            if (hit) break;
        }
    }
    { int idx = 0; enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [&](const Bullet&){ return !eBulletAlive[idx++]; }), enemyBullets.end()); }

    // Seviye atlama
    if (enemies.empty() && gameState == State::Playing) {
        level++;
        levelText.setString("Seviye: " + std::to_string(level));
        swarmSpeed        += 20.f;
        swarmMoveInterval  = 0.8f;
        dropPending        = false;
        enemyShootInterval *= 0.9f;
        initLevel();
        bullets.clear();
        enemyBullets.clear();
    }
}

void GameManager::render() {
    window.clear(sf::Color::Black);
 
    for (auto& enemy   : enemies)      enemy.draw(window);
    for (auto& bullet  : bullets)      bullet.draw(window);
    for (auto& eBullet : enemyBullets) eBullet.draw(window);
    for (auto& barrier : barriers)     barrier.draw(window);
 
    // playerVisible false iken oyuncuyu çizme — yanıp sönme
    if (playerVisible) player.draw(window);
 
    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);
 
    if (gameState == State::GameOver) window.draw(gameOverText);
 
    window.display();
}