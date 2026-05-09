#include "GameManager.h"
#include <cstdlib>
#include <ctime>

GameManager::GameManager() : window(sf::VideoMode(800, 600), "Space Invaders - YZM104") {
    srand(time(0));
    score = 0;
    lives = 3;
    level = 1;
    isGameOver = false;

    if (!font.loadFromFile("C:/Windows/Fonts/consola.ttf")) {
        printf("Font yuklenemedi!\n");
    }

    scoreText.setFont(font); scoreText.setCharacterSize(20); scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, 20.f); scoreText.setString("Skor: 0");

    livesText.setFont(font); livesText.setCharacterSize(20); livesText.setFillColor(sf::Color::White);
    livesText.setPosition(650.f, 20.f); livesText.setString("Can: 3");

    levelText.setFont(font); levelText.setCharacterSize(20); levelText.setFillColor(sf::Color::White);
    levelText.setPosition(350.f, 20.f); levelText.setString("Seviye: 1");

    gameOverText.setFont(font); gameOverText.setCharacterSize(80); gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");
    sf::FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(800.0f / 2.0f, 600.0f / 2.0f);

    barriers.push_back(Barrier(100.0f, 450.0f));
    barriers.push_back(Barrier(350.0f, 450.0f));
    barriers.push_back(Barrier(600.0f, 450.0f));

    swarmSpeed = 100.0f;
    swarmDirection = 1;
    dropDistance = 20.0f;
    enemyShootTimer = 0.0f;
    enemyShootInterval = 1.0f;

    initLevel();
}

void GameManager::initLevel() {
    enemies.clear();
    int rows = 4; int cols = 8;
    float startX = 50.0f; float startY = 50.0f;
    float spacingX = 60.0f; float spacingY = 50.0f;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            enemies.push_back(Enemy(startX + j * spacingX, startY + i * spacingY));
        }
    }
}

void GameManager::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void GameManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
    }
}

void GameManager::update(float deltaTime) {
    if (isGameOver) return;

    player.update(deltaTime, bullets);

    // Oyuncu mermileri ve çarpışmalar
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].update(deltaTime);
        bool bulletDestroyed = false;

        for (int j = 0; j < enemies.size(); j++) {
            if (bullets[i].getBounds().intersects(enemies[j].getBounds())) {
                enemies[j].takeDamage(1);
                bulletDestroyed = true;
                score += 10;
                scoreText.setString("Skor: " + std::to_string(score));
                break;
            }
        }

        if (!bulletDestroyed) {
            for (auto& barrier : barriers) {
                auto& blocks = barrier.getBlocks();
                for (int k = 0; k < blocks.size(); k++) {
                    if (bullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                        blocks.erase(blocks.begin() + k);
                        bulletDestroyed = true;
                        break;
                    }
                }
                if (bulletDestroyed) break;
            }
        }

        if (bulletDestroyed || bullets[i].getY() < 0) {
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }

    // Düşman ordusunun hareketi
    bool hitWall = false;
    for (auto& enemy : enemies) {
        enemy.move(swarmSpeed * swarmDirection * deltaTime, 0);
        if (enemy.getX() <= 0 || enemy.getX() >= 800 - 40.0f) hitWall = true;
    }

    if (hitWall) {
        swarmDirection *= -1;
        for (auto& enemy : enemies) {
            enemy.move(swarmSpeed * swarmDirection * deltaTime, dropDistance);
        }
    }

    // Düşman ateşi
    enemyShootTimer += deltaTime;
    if (enemyShootTimer >= enemyShootInterval && !enemies.empty()) {
        int randomIndex = rand() % enemies.size();
        float startX = enemies[randomIndex].getX() + 20.0f;
        float startY = enemies[randomIndex].getY() + 30.0f;
        enemyBullets.push_back(Bullet(startX, startY, 1.0f, sf::Color::Red));
        enemyShootTimer = 0.0f;
        enemyShootInterval = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
    }

    // Ölü düşmanları sil
    for (int i = 0; i < enemies.size(); i++) {
        if (!enemies[i].isAlive()) {
            enemies.erase(enemies.begin() + i);
            i--;
        }
    }

    // Düşman mermileri ve çarpışmalar
    for (int i = 0; i < enemyBullets.size(); i++) {
        enemyBullets[i].update(deltaTime);
        bool eBulletDestroyed = false;

        if (enemyBullets[i].getBounds().intersects(player.getBounds())) {
            lives--;
            livesText.setString("Can: " + std::to_string(lives));
            eBulletDestroyed = true;
            if (lives <= 0) {
                isGameOver = true;
                window.setTitle("Space Invaders - GAME OVER!");
            }
        }

        if (!eBulletDestroyed) {
            for (auto& barrier : barriers) {
                auto& blocks = barrier.getBlocks();
                for (int k = 0; k < blocks.size(); k++) {
                    if (enemyBullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                        blocks.erase(blocks.begin() + k);
                        eBulletDestroyed = true;
                        break;
                    }
                }
                if (eBulletDestroyed) break;
            }
        }

        if (eBulletDestroyed || enemyBullets[i].getY() > 600) {
            enemyBullets.erase(enemyBullets.begin() + i);
            i--;
        }
    }

    // Seviye atlama
    if (enemies.empty() && !isGameOver) {
        level++;
        levelText.setString("Seviye: " + std::to_string(level));
        swarmSpeed += 20.0f;
        enemyShootInterval *= 0.9f;
        initLevel();
        bullets.clear();
        enemyBullets.clear();
    }
}

void GameManager::render() {
    window.clear(sf::Color::Black);

    player.draw(window);
    for (auto& bullet : bullets) bullet.draw(window);
    for (auto& enemy : enemies) enemy.draw(window);
    for (auto& eBullet : enemyBullets) eBullet.draw(window);
    for (auto& barrier : barriers) barrier.draw(window);

    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);

    if (isGameOver) window.draw(gameOverText);

    window.display();
}