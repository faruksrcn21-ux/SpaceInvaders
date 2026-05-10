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
    srand((unsigned int)time(0));

    fontLoaded = font.loadFromFile("assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/consola.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("/Library/Fonts/Courier New.ttf");
    if (!fontLoaded)
        printf("[UYARI] Font yuklenemedi! 'assets/font.ttf' ekleyin.\n");
    

    // HUD metinleri
    scoreText.setFont(font); scoreText.setCharacterSize(20); scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, 10.f); scoreText.setString("Skor: 0");

    livesText.setFont(font); livesText.setCharacterSize(20); livesText.setFillColor(sf::Color::White);
    livesText.setPosition(650.f, 10.f); livesText.setString("Can: 3");

    levelText.setFont(font); levelText.setCharacterSize(20); levelText.setFillColor(sf::Color::White);
    levelText.setPosition(350.f, 10.f); levelText.setString("Seviye: 1");

    // Game Over
    gameOverText.setFont(font); gameOverText.setCharacterSize(72); gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");
    centreX(gameOverText, 800.f); gameOverText.setPosition(gameOverText.getPosition().x, 200.f);

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
    sound_.startMusic(); //müziği başlat
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
    explosions.clear(); // Patlama particle sistemi
    kamikazeTimer_    = 0.f;
    kamikazeInterval_ = 8.f;


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

            // durum geçişleri
            // Menu  → Enter → Playing (resetGame ile)
            // GameOver → Enter → Playing
            // Win      → Enter → Playing
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

    // atış sesi — bullet eklendiyse çal
    int bulletsBefore = (int)bullets.size();
    player.update(DeltaTime, bullets);
    if ((int)bullets.size() > bulletsBefore)
        sound_.playShoot();
    

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
                // düşman ölünce patlama — rengi tipine göre (DISABLED)
                sf::Color expColor;
                switch (enemy.getType()) {
                     case EnemyType::A: expColor = sf::Color(255, 80,  220); break;
                     case EnemyType::B: expColor = sf::Color(255, 160,  40); break;
                     case EnemyType::C: expColor = sf::Color(255,  80,  80); break;
                 }
                 explosions.emplace_back(
                     enemy.getX() + 20.f, enemy.getY() + 15.f, expColor, 14);
 
                 sound_.playExplosion(); // düşman vurulma sesi

                hit = true; break;
            }
        }
        if (hit) { bulletAlive[i] = false; continue; }

        for (auto& barrier : barriers) {
            auto& blocks = barrier.getBlocks();
            auto& hpList = barrier.getBlockHp();
            for (int k = 0; k < (int)blocks.size(); k++) {
                if (hpList[k] <= 0) continue;
                if (bullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                    hpList[k]--;
                    if (hpList[k] <= 0)
                        blocks[k].setSize(sf::Vector2f(0.f, 0.f)); // görünmez
                    bulletAlive[i] = false; hit = true; break;                }
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
            for (auto& e : enemies)
                if (!e.isKamikaze()) e.move(0.f, dropDistance);
            swarmDirection *= -1; dropPending = false;
        } else {
            float step = swarmSpeed * swarmDirection * swarmMoveInterval;
            bool hitWall = false;
            for (auto& e : enemies) {
                if (e.isKamikaze()) continue;  // kamikaze duvar kontrolü dışı
                float nx = e.getX() + step;
                if (nx <= 0.f || nx >= 760.f) { hitWall = true; break; }
            }
            if (hitWall) dropPending = true;
            // kamikaze olan düşmanlar formasyon dışı
            else for (auto& e : enemies)
                if (!e.isKamikaze()) e.move(step, 0.f);
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
        sound_.playEnemyShoot(); // düşman ateş sesi                              
        enemyShootTimer = 0.f;
        enemyShootInterval = 0.5f + (float)rand() / (float)(RAND_MAX / 1.5f);
    }

    // Ölü düşmanları listeden temizle
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e){ return !e.isAlive(); }), enemies.end());

    // Kamikaze sistemi
    // Her 8 saniyede bir, hayatta olan Tip C düşmanlardan rastgele
    // biri kamikaze moduna geçer ve oyuncuya doğru dalar.
    // Kamikaze düşman formasyon hareketinden bağımsız hareket eder.
    // Oyuncuya veya ekran dışına ulaşınca ölür.
    kamikazeTimer_ += DeltaTime;
    if (kamikazeTimer_ >= kamikazeInterval_) {
        kamikazeTimer_ = 0.f;
 
        // Tip C ve henüz kamikaze olmayan düşmanları topla
        std::vector<int> candidates;
        for (int i = 0; i < (int)enemies.size(); i++) {
            if (enemies[i].getType() == EnemyType::C &&
                !enemies[i].isKamikaze())
                candidates.push_back(i);
        }
 
        if (!candidates.empty()) {
            int pick = candidates[rand() % candidates.size()];
            // Oyuncunun orta noktasını hedef al
            sf::FloatRect pb = player.getBounds();
            float tx = pb.left + pb.width  / 2.f;
            float ty = pb.top  + pb.height / 2.f;
            enemies[pick].activateKamikaze(tx, ty);
        }
 
        // Seviye arttıkça daha sık kamikaze
        kamikazeInterval_ = std::max(3.f, 8.f - level * 0.8f);
    }
 
    // Kamikaze düşmanları güncelle
    for (auto& e : enemies) {
        if (!e.isKamikaze()) continue;
        e.updateKamikaze(DeltaTime);
 
        // Oyuncuya çarptı mı?
        if (invincibleTimer <= 0.f &&
            e.getBounds().intersects(player.getBounds()))
        {
            e.takeDamage(1);  // kamikaze düşman da ölür
            lives--;
            livesText.setString("Can: " + std::to_string(lives));
 
            sf::FloatRect pb = player.getBounds();
            explosions.emplace_back(
                pb.left + pb.width  / 2.f,
                pb.top  + pb.height / 2.f,
                sf::Color(255, 200, 80), 16);  // büyük turuncu patlama
            sound_.playExplosion();
            sound_.playPlayerHit();
 
            if (lives <= 0) {
                gameState = State::GameOver;
            } else {
                invincibleTimer = INV_DURATION;
                blinkTimer = 0.f; playerVisible = true;
            }
        }
 
        // Ekran dışına çıktıysa öldür
        if (e.hasReachedTarget()) e.takeDamage(1);
    }
 
    // Ölü kamikazeler de temizlenir (yukarıdaki remove_if zaten halleder)
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e){ return !e.isAlive(); }), enemies.end());

        // patlama güncelle + biten patlama sil
    for (auto& exp : explosions) exp.update(DeltaTime);
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
         [](const Explosion& e){ return e.isFinished(); }), explosions.end());
 
    // Düşman tabana inince Game Over
    if (gameState == State::Playing) {
        for (auto& enemy : enemies) {
            if (!enemy.isKamikaze() && enemy.getY() + 30.f >= 420.f) {  // 30 = düşman yüksekliği
                gameState = State::GameOver;
                break;
            }
        }
    }

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

            // oyuncu vurulunca küçük beyaz patlama
            sf::FloatRect pb = player.getBounds();
            explosions.emplace_back(
                pb.left + pb.width / 2.f,
                pb.top  + pb.height / 2.f,
                sf::Color(255, 255, 180), 8);
            sound_.playPlayerHit(); // oyuncu hasar sesi
                
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
        swarmSpeed        += 20.f;
        swarmMoveInterval  = 0.8f;
        dropPending        = false;
        swarmDirection     = 1;
        enemyShootInterval = std::max(0.3f, enemyShootInterval * 0.85f);
        bullets.clear();
        enemyBullets.clear();
 
        // Bariyerleri kısmen onar (yeni seviyede avantaj)
        barriers.clear();
        barriers.push_back(Barrier(100.f, 450.f));
        barriers.push_back(Barrier(350.f, 450.f));
        barriers.push_back(Barrier(600.f, 450.f));

        if (level > 3) {
            // 3 seviyeyi geçtiyse kazandı
            winSubText.setString("Skor: " + std::to_string(score) +
                                 "   Seviye: " + std::to_string(level - 1));
            centreX(winSubText, 800.f);
            gameState = State::Win;
        } else {
            // Yeni dalga
            initLevel();
        }
    }
}

void GameManager::render() {
    window.clear(sf::Color::Black);

    if (gameState == State::Menu) {
        // Ana menü ekranı
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
        for (auto& exp : explosions)       exp.draw(window);

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