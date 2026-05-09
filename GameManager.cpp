#include "GameManager.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>  // std::remove_if
#include <cstdio>     // printf

GameManager::GameManager() : window(sf::VideoMode(800, 600), "Space Invaders - YZM104") {
    srand(time(0));
    score = 0;
    lives = 3;
    level = 1;
    isGameOver = false;


    // ESKİ HATA:
    //   "C:/Windows/Fonts/consola.ttf" sadece Windows'ta çalışıyor.
    //   Linux/Mac'te hata verip font olmadan devam ediyordu → tüm
    //   metinler (skor, can, level) ekranda görünmüyordu.
    //
    // YENİ MANTIK:
    //   Önce proje klasöründeki assets/font.ttf denenir.
    //   Bulunamazsa işletim sistemine göre sistem fontları denenir.
    //   Hiçbiri bulunamazsa konsola açık bir hata mesajı yazılır.
    //   assets/font.ttf için herhangi ücretsiz bir .ttf kopyalanabilir
    //   (örn. DejaVuSansMono.ttf → assets/font.ttf olarak yeniden adlandır).

    bool fontLoaded = false;

    // 1. Projeye dahil edilmiş font (tüm platformlarda çalışır)
    fontLoaded = font.loadFromFile("assets/font.ttf");

    // 2. Windows sistem fontları
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/consola.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    // 3. Linux sistem fontları
    if (!fontLoaded)
        fontLoaded = font.loadFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile(
            "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile(
            "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf");

    // 4. macOS sistem fontları
    if (!fontLoaded)
        fontLoaded = font.loadFromFile(
            "/Library/Fonts/Courier New.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile(
            "/System/Library/Fonts/Supplemental/Courier New.ttf");

    if (!fontLoaded)
        printf("[UYARI] Font yuklenemedi! Metinler gorunmeyecek.\n"
               "        Cozum: Herhangi bir .ttf dosyasini 'assets/font.ttf'\n"
               "        olarak proje klasorune kopyalayin.\n");

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

    swarmSpeed         = 100.0f;
    swarmDirection     = 1;
    dropDistance       = 20.0f;
    enemyShootTimer    = 0.0f;
    enemyShootInterval = 1.0f;

    dropPending        = false;
    swarmMoveTimer     = 0.0f;
    swarmMoveInterval  = 0.8f;

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
    }
}

void GameManager::update(float deltaTime) {
    if (isGameOver) return;

    player.update(deltaTime, bullets);


    // ESKİ HATA:
    //   for (int i = 0; i < bullets.size(); i++) içinde
    //   bullets.erase(bullets.begin() + i) çağrısı iterator'ı geçersiz
    //   kılıyor. Aynı döngüde devam etmek Undefined Behaviour.
    
    // YENİ MANTIK:
    //   1. Tüm mermileri güncelle (konum, çarpışma).
    //   2. Çarpışan veya ekran dışına çıkan mermileri "destroyed" flag'i
    //      ile işaretle — döngü içinde silme yok.
    //   3. Döngü bittikten sonra remove_if + erase ile tek seferde sil.
    //      Bu C++ standardına uygun, güvenli "erase-remove" kalıbıdır.

    std::vector<bool> bulletAlive(bullets.size(), true);

    for (int i = 0; i < (int)bullets.size(); i++) {
        bullets[i].update(deltaTime);

        // Ekran dışına çıktıysa işaretle
        if (bullets[i].getY() < 0) {
            bulletAlive[i] = false;
            continue;
        }

        // Düşmanlara çarpışma
        bool hit = false;
        for (auto& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            if (bullets[i].getBounds().intersects(enemy.getBounds())) {
                enemy.takeDamage(1);
                score += 10;
                scoreText.setString("Skor: " + std::to_string(score));
                hit = true;
                break;
            }
        }
        if (hit) { bulletAlive[i] = false; continue; }

        // Bariyerlere çarpışma
        for (auto& barrier : barriers) {
            auto& blocks = barrier.getBlocks();
            for (int k = 0; k < (int)blocks.size(); k++) {
                if (bullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                    blocks.erase(blocks.begin() + k);  // bariyer bloğu silinir — bu güvenli
                    bulletAlive[i] = false;
                    hit = true;
                    break;
                }
            }
            if (hit) break;
        }
    }

    // Ölü mermileri tek seferde sil (erase-remove kalıbı)
    {
        int idx = 0;
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                [&](const Bullet&) { return !bulletAlive[idx++]; }),
            bullets.end()
        );
    }

    // Formasyon hareketi 
    swarmMoveTimer += deltaTime;
    if (swarmMoveTimer >= swarmMoveInterval) {
        swarmMoveTimer = 0.0f;

        if (dropPending) {
            for (auto& enemy : enemies)
                enemy.move(0.0f, dropDistance);
            swarmDirection *= -1;
            dropPending = false;
        } else {
            float step = swarmSpeed * swarmDirection * swarmMoveInterval;
            bool hitWall = false;
            for (auto& enemy : enemies) {
                float nextX = enemy.getX() + step;
                if (nextX <= 0.0f || nextX >= 800.0f - 40.0f) {
                    hitWall = true;
                    break;
                }
            }
            if (hitWall) {
                dropPending = true;
            } else {
                for (auto& enemy : enemies)
                    enemy.move(step, 0.0f);
            }

            int aliveCount = (int)enemies.size();
            int totalCount = 4 * 8;
            float ratio = 1.0f - (float)aliveCount / (float)totalCount;
            swarmMoveInterval = 0.8f * (1.0f - ratio * 0.75f);
            if (swarmMoveInterval < 0.05f) swarmMoveInterval = 0.05f;
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

    // Ölü düşmanları güvenli sil
    // ESKİ HATA: for döngüsü içinde erase() — UB
    // YENİ: remove_if + erase kalıbı
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.isAlive(); }),
        enemies.end()
    );

    // Düşman mermileri — aynı güvenli kalıp
    std::vector<bool> eBulletAlive(enemyBullets.size(), true);

    for (int i = 0; i < (int)enemyBullets.size(); i++) {
        enemyBullets[i].update(deltaTime);

        // Ekran dışına çıktıysa
        if (enemyBullets[i].getY() > 600) {
            eBulletAlive[i] = false;
            continue;
        }

        // Oyuncuya çarpışma
        if (enemyBullets[i].getBounds().intersects(player.getBounds())) {
            lives--;
            livesText.setString("Can: " + std::to_string(lives));
            eBulletAlive[i] = false;
            if (lives <= 0) {
                isGameOver = true;
                window.setTitle("Space Invaders - GAME OVER!");
            }
            continue;
        }

        // Bariyerlere çarpışma
        bool hit = false;
        for (auto& barrier : barriers) {
            auto& blocks = barrier.getBlocks();
            for (int k = 0; k < (int)blocks.size(); k++) {
                if (enemyBullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                    blocks.erase(blocks.begin() + k);
                    eBulletAlive[i] = false;
                    hit = true;
                    break;
                }
            }
            if (hit) break;
        }
    }

    // Ölü düşman mermilerini tek seferde sil
    {
        int idx = 0;
        enemyBullets.erase(
            std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                [&](const Bullet&) { return !eBulletAlive[idx++]; }),
            enemyBullets.end()
        );
    }

    // Seviye atlama 
    if (enemies.empty() && !isGameOver) {
        level++;
        levelText.setString("Seviye: " + std::to_string(level));
        swarmSpeed        += 20.0f;
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

    player.draw(window);
    for (auto& bullet  : bullets)      bullet.draw(window);
    for (auto& enemy   : enemies)      enemy.draw(window);
    for (auto& eBullet : enemyBullets) eBullet.draw(window);
    for (auto& barrier : barriers)     barrier.draw(window);

    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);

    if (isGameOver) window.draw(gameOverText);

    window.display();
}