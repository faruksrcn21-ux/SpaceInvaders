#include "GameManager.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstdio>
#include "Enemy.h"

    // Yardımcı: metni yatayda ortala
static void centreX(sf::Text& t, float windowWidth) {
    sf::FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, 0.f);
    t.setPosition(windowWidth / 2.f, t.getPosition().y);
}

GameManager::GameManager() : window(sf::VideoMode(800, 600), "Space Invaders") {
    srand(time(0));

    // Font yükleme
    fontLoaded = false;
    fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("../assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("../../assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/consola.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/courier.ttf");
    if (!fontLoaded)
        printf("[UYARI] Font yuklenemedi! Text'ler gorunmeyecek.\n");


    // HUD metinleri — sadece font yüklenmişse ayarla
    if (fontLoaded) {
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
        menuTitleText.setFont(font); menuTitleText.setCharacterSize(64);
        menuTitleText.setFillColor(sf::Color::Cyan);
        menuTitleText.setString("SPACE INVADERS");
        centreX(menuTitleText, 800.f); menuTitleText.setPosition(menuTitleText.getPosition().x, 160.f);
 
        menuSubText.setFont(font); menuSubText.setCharacterSize(22);
        menuSubText.setFillColor(sf::Color::White);
        menuSubText.setString("Ok tuslari: Hareket     Bosluk: Ates");
        centreX(menuSubText, 800.f); menuSubText.setPosition(menuSubText.getPosition().x, 290.f);
        
        
        // Kazanma metni
        winText.setFont(font); winText.setCharacterSize(72);
        winText.setFillColor(sf::Color::Green);
        winText.setString("KAZANDINIZ!");
        centreX(winText, 800.f); winText.setPosition(winText.getPosition().x, 200.f);
 
        winSubText.setFont(font); winSubText.setCharacterSize(28);
        winSubText.setFillColor(sf::Color::White);
        // İçerik resetGame'den sonra güncellenecek
        winSubText.setString(""); centreX(winSubText, 800.f);
        winSubText.setPosition(winSubText.getPosition().x, 300.f);


        // Ortak "yeniden başlat" ipucu
        restartHintText.setFont(font); restartHintText.setCharacterSize(24);
        restartHintText.setFillColor(sf::Color(180, 180, 180));
        restartHintText.setString("ENTER: Tekrar oyna     ESC: Cikis");
        centreX(restartHintText, 800.f); restartHintText.setPosition(restartHintText.getPosition().x, 400.f);
    }


    // Bariyerler
    barriers.push_back(Barrier(100.0f, 450.0f));
    barriers.push_back(Barrier(350.0f, 450.0f));
    barriers.push_back(Barrier(600.0f, 450.0f));
 
    swarmSpeed = 100.f; swarmDirection = 1; dropDistance = 20.f;
    enemyShootTimer = 0.f; enemyShootInterval = 1.f;
    dropPending = false; swarmMoveTimer = 0.f; swarmMoveInterval = 0.8f;
 
    invincibleTimer = 0.f; blinkTimer = 0.f; playerVisible = true;


    // oyun menüden başlar
    gameState = State::Menu;
    score = 0; lives = 3; level = 1; isGameOver = false;


    initLevel();
}


    // resetGame — tüm oyun değişkenlerini sıfırla
    void GameManager::resetGame() {
    score = 0; lives = 3; level = 1; isGameOver = false;
    scoreText.setString("Skor: 0");
    livesText.setString("Can: 3");
    levelText.setString("Seviye: 1");
 
    swarmSpeed = 100.f; swarmDirection = 1;
    swarmMoveTimer = 0.f; swarmMoveInterval = 0.8f; dropPending = false;
    enemyShootTimer = 0.f; enemyShootInterval = 1.f;
 
    invincibleTimer = 0.f; blinkTimer = 0.f; playerVisible = true;
 
    bullets.clear(); enemyBullets.clear();
 
    barriers.clear();
    barriers.push_back(Barrier(100.f, 450.f));
    barriers.push_back(Barrier(350.f, 450.f));
    barriers.push_back(Barrier(600.f, 450.f));
 
    initLevel();
    gameState = State::Playing;
}
    
// initLevel — düşmanları oluştur
void GameManager::initLevel() {
    enemies.clear();
    for (int i = 0; i < 4; i++) {
        EnemyType type = (i == 0) ? EnemyType::A : (i <= 2) ? EnemyType::B : EnemyType::C;
        for (int j = 0; j < 8; j++)
            enemies.push_back(Enemy(50.f + j * 60.f, 50.f + i * 50.f, type));
    }
}
 
// run — ana oyun döngüsü
void GameManager::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        processEvents();
        update(dt);
        render();
    }
}


void GameManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
 
        if (event.type == sf::Event::KeyPressed) {
            // ESC her zaman kapatır
            if (event.key.code == sf::Keyboard::Escape) window.close();
 
            // ------------------------------------------------------------------
            // durum geçişleri
            // Menu  → Enter → Playing (resetGame ile)
            // GameOver → Enter → Playing
            // Win      → Enter → Playing
            // ------------------------------------------------------------------
            if (event.key.code == sf::Keyboard::Enter) {
                if (gameState == State::Menu ||
                    gameState == State::GameOver ||
                    gameState == State::Win)
                {
                    resetGame();
                }
            }
        }
    }
}


void GameManager::update(float DeltaTime) {
    // Menü ve bitiş ekranlarında oyun mantığı çalışmaz
    if (gameState != State::Playing) return;
 
    player.update(DeltaTime, bullets);
 
    // Dokunulmazlık zamanlayıcısı
    if (invincibleTimer > 0.f) {
        invincibleTimer -= DeltaTime;
        blinkTimer += DeltaTime;
        if (blinkTimer >= BLINK_RATE) { blinkTimer = 0.f; playerVisible = !playerVisible; }
        if (invincibleTimer <= 0.f)   { invincibleTimer = 0.f; playerVisible = true; }
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
                    blocks.erase(blocks.begin() + k); bulletAlive[i] = false; hit = true; break;
                }
            }
            if (hit) break;
        }
    }
    { int idx = 0; bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [&](const Bullet&){ return !bulletAlive[idx++]; }), bullets.end()); }


    // Formasyon
    swarmMoveTimer += DeltaTime;
    if (swarmMoveTimer >= swarmMoveInterval) {
        swarmMoveTimer = 0.f;
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
            float ratio = 1.f - (float)enemies.size() / 32.f;
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
        enemyShootTimer = 0.f;
        enemyShootInterval = 0.5f + (float)rand() / (float)(RAND_MAX / 1.5f);
    }


    // Ölü düşmanları listeden temizle
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e){ return !e.isAlive(); }), enemies.end());

 
    // Düşman mermileri
    std::vector<bool> eBulletAlive(enemyBullets.size(), true);
    for (int i = 0; i < (int)enemyBullets.size(); i++) {
        enemyBullets[i].update(DeltaTime);
        if (enemyBullets[i].getY() > 600) { eBulletAlive[i] = false; continue; }
 
        if (invincibleTimer <= 0.f &&
            enemyBullets[i].getBounds().intersects(player.getBounds()))
        {
            lives--;
            livesText.setString("Can: " + std::to_string(lives));
            eBulletAlive[i] = false;
            if (lives <= 0) {
                gameState = State::GameOver;
            } else {
                invincibleTimer = INV_DURATION;
                blinkTimer = 0.f; playerVisible = true;
            }
            continue;
        }
        bool hit = false;
        for (auto& barrier : barriers) {
            auto& blocks = barrier.getBlocks();
            for (int k = 0; k < (int)blocks.size(); k++) {
                if (enemyBullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                    blocks.erase(blocks.begin() + k); eBulletAlive[i] = false; hit = true; break;
                }
            }
            if (hit) break;
        }
    }
    { int idx = 0; enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [&](const Bullet&){ return !eBulletAlive[idx++]; }), enemyBullets.end()); }

   
    // Kazanma kontrolü — tüm düşmanlar öldüyse Win ekranı
    if (enemies.empty() && gameState == State::Playing) {
        level++;
        levelText.setString("Seviye: " + std::to_string(level));
 
        // Her 3 seviyede bir yeni level yerine win göster
        // (basit tek-level oyun; sonraki committe multi-level yapılabilir)
        winSubText.setString("Skor: " + std::to_string(score) +
                             "   Seviye: " + std::to_string(level - 1));
        centreX(winSubText, 800.f);
        gameState = State::Win;
    }
}


void GameManager::render() {
    window.clear(sf::Color::Black);
 
    if (gameState == State::Menu) {
        // ------------------------------------------------------------------
        // COMMIT 6: Ana menü ekranı
        // ------------------------------------------------------------------
        window.draw(menuTitleText);
        window.draw(menuSubText);
 
        // Kontrol listesi
        sf::Text ctrl;
        ctrl.setFont(font); ctrl.setCharacterSize(18);
        ctrl.setFillColor(sf::Color(160, 160, 160));
 
        const char* lines[] = {
            "Pembe dusmanlar : 30 puan",
            "Turuncu dusmanlar: 20 puan",
            "Kirmizi dusmanlar: 10 puan"
        };
        for (int i = 0; i < 3; i++) {
            ctrl.setString(lines[i]);
            centreX(ctrl, 800.f);
            ctrl.setPosition(ctrl.getPosition().x, 360.f + i * 30.f);
            window.draw(ctrl);
        }
 
        // Enter ipucu
        sf::Text enterHint;
        enterHint.setFont(font); enterHint.setCharacterSize(26);
        enterHint.setFillColor(sf::Color::Yellow);
        enterHint.setString("ENTER ile baslat");
        centreX(enterHint, 800.f); enterHint.setPosition(enterHint.getPosition().x, 470.f);
        window.draw(enterHint);
 
    } else if (gameState == State::Playing) {
        for (auto& enemy   : enemies)      enemy.draw(window);
        for (auto& bullet  : bullets)      bullet.draw(window);
        for (auto& eBullet : enemyBullets) eBullet.draw(window);
        for (auto& barrier : barriers)     barrier.draw(window);
        if (playerVisible) player.draw(window);
 
        window.draw(scoreText);
        window.draw(livesText);
        window.draw(levelText);
 
    } else if (gameState == State::GameOver) {
        window.draw(gameOverText);
        // Skoru da göster
        sf::Text finalScore;
        finalScore.setFont(font); finalScore.setCharacterSize(28);
        finalScore.setFillColor(sf::Color::White);
        finalScore.setString("Skor: " + std::to_string(score));
        centreX(finalScore, 800.f); finalScore.setPosition(finalScore.getPosition().x, 310.f);
        window.draw(finalScore);
        window.draw(restartHintText);
 
    } else if (gameState == State::Win) {
        window.draw(winText);
        window.draw(winSubText);
        window.draw(restartHintText);
    }
 
    window.display();
}