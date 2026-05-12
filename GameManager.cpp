#include "GameManager.h"
#include "Enemy.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>

// Yardımcı: metni yatayda ortala
static void centreX(sf::Text &t, float windowWidth) {
  sf::FloatRect r = t.getLocalBounds();
  t.setOrigin(r.left + r.width / 2.f, 0.f);
  t.setPosition(windowWidth / 2.f, t.getPosition().y);
}

GameManager::GameManager()
    : window(sf::VideoMode((unsigned int)WINDOW_WIDTH,
                           (unsigned int)WINDOW_HEIGHT),
             "Space Invaders") {
  srand((unsigned int)time(0));

  // fontLoaded artık local değişken (member field değil)
  bool fontLoaded = font.loadFromFile("assets/font.ttf");
  if (!fontLoaded)
    fontLoaded = font.loadFromFile("C:/Windows/Fonts/consola.ttf");
  if (!fontLoaded)
    fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
  if (!fontLoaded)
    fontLoaded = font.loadFromFile(
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
  if (!fontLoaded)
    fontLoaded = font.loadFromFile(
        "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf");
  if (!fontLoaded)
    fontLoaded = font.loadFromFile("/Library/Fonts/Courier New.ttf");
  if (!fontLoaded)
    printf("[UYARI] Font yuklenemedi! 'assets/font.ttf' ekleyin.\n");

  // HUD metinleri
  scoreText.setFont(font);
  scoreText.setCharacterSize(20);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setPosition(20.f, 10.f);
  scoreText.setString("Skor: 0");

  livesText.setFont(font);
  livesText.setCharacterSize(20);
  livesText.setFillColor(sf::Color::White);
  livesText.setPosition(650.f, 10.f);
  livesText.setString("Can: 3");

  levelText.setFont(font);
  levelText.setCharacterSize(20);
  levelText.setFillColor(sf::Color::White);
  levelText.setPosition(350.f, 10.f);
  levelText.setString("Seviye: 1");

  // Game Over
  gameOverText.setFont(font);
  gameOverText.setCharacterSize(72);
  gameOverText.setFillColor(sf::Color::Red);
  gameOverText.setString("GAME OVER");
  centreX(gameOverText, WINDOW_WIDTH);
  gameOverText.setPosition(gameOverText.getPosition().x, 200.f);

  // Menü metinleri
  menuTitleText.setFont(font);
  menuTitleText.setCharacterSize(64);
  menuTitleText.setFillColor(sf::Color::Cyan);
  menuTitleText.setString("SPACE INVADERS");
  centreX(menuTitleText, WINDOW_WIDTH);
  menuTitleText.setPosition(menuTitleText.getPosition().x, 160.f);

  menuSubText.setFont(font);
  menuSubText.setCharacterSize(22);
  menuSubText.setFillColor(sf::Color::White);
  menuSubText.setString("Ok tuslari: Hareket     Bosluk: Ates");
  centreX(menuSubText, WINDOW_WIDTH);
  menuSubText.setPosition(menuSubText.getPosition().x, 290.f);

  // Ortak "yeniden başlat" ipucu
  restartHintText.setFont(font);
  restartHintText.setCharacterSize(24);
  restartHintText.setFillColor(sf::Color(180, 180, 180));
  restartHintText.setString("ENTER: Tekrar oyna     ESC: Cikis");
  centreX(restartHintText, WINDOW_WIDTH);
  restartHintText.setPosition(restartHintText.getPosition().x, 400.f);

  // Pause ekranı metinleri
  pauseText.setFont(font);
  pauseText.setCharacterSize(64);
  pauseText.setFillColor(sf::Color::Yellow);
  pauseText.setString("DURAKLATILDI");
  centreX(pauseText, WINDOW_WIDTH);
  pauseText.setPosition(pauseText.getPosition().x, 200.f);

  pauseHintText.setFont(font);
  pauseHintText.setCharacterSize(22);
  pauseHintText.setFillColor(sf::Color(180, 180, 180));
  pauseHintText.setString("P: Devam et     M: Ses Ac/Kapat     ESC: Cikis");
  centreX(pauseHintText, WINDOW_WIDTH);
  pauseHintText.setPosition(pauseHintText.getPosition().x, 320.f);

  // Ses durumu göstergesi (menü ve pause ekranında)
  soundStatusText.setFont(font);
  soundStatusText.setCharacterSize(18);
  soundStatusText.setFillColor(sf::Color(120, 220, 120));
  soundStatusText.setString("Ses: ACIK  (M ile degistir)");
  centreX(soundStatusText, WINDOW_WIDTH);
  soundStatusText.setPosition(soundStatusText.getPosition().x, 550.f);

  // High Score metni
  highScoreText.setFont(font);
  highScoreText.setCharacterSize(22);
  highScoreText.setFillColor(sf::Color::Yellow);
  loadHighScore();
  highScoreText.setString("En Yuksek Skor: " + std::to_string(highScore));
  centreX(highScoreText, WINDOW_WIDTH);
  highScoreText.setPosition(highScoreText.getPosition().x, 240.f);

  // Bariyerler
  barriers.push_back(Barrier(100.f, BARRIER_Y));
  barriers.push_back(Barrier(350.f, BARRIER_Y));
  barriers.push_back(Barrier(600.f, BARRIER_Y));

  swarmSpeed = 100.f;
  swarmDirection = 1;
  dropDistance = 20.f;
  enemyShootTimer = 0.f;
  enemyShootInterval = 1.f;
  dropPending = false;
  swarmMoveTimer = 0.f;
  swarmMoveInterval = 0.8f;

  invincibleTimer = 0.f;
  blinkTimer = 0.f;
  playerVisible = true;

  // oyun menüden başlar
  gameState = State::Menu;
  score = 0;
  lives = 3;
  level = 1;

  initLevel();
  initStars();         // parallax yıldız arkaplanı
  sound_.startMusic(); // müziği başlat
}

void GameManager::playerHit(bool isKamikaze) {
  lives--;
  livesText.setString("Can: " + std::to_string(lives));

  sf::FloatRect pb = player.getBounds();
  if (isKamikaze) {
    explosions.emplace_back(pb.left + pb.width / 2.f, pb.top + pb.height / 2.f,
                            sf::Color(255, 200, 80), 16);
    sound_.playExplosion();
  } else {
    explosions.emplace_back(pb.left + pb.width / 2.f, pb.top + pb.height / 2.f,
                            sf::Color(255, 255, 180), 8);
  }
  sound_.playPlayerHit();

  if (lives <= 0) {
    gameState = State::GameOver;
  } else {
    invincibleTimer = INV_DURATION;
    blinkTimer = 0.f;
    playerVisible = true;
  }
}

bool GameManager::checkBarrierCollision(const sf::FloatRect &bounds) {
  for (auto &barrier : barriers) {
    auto &blocks = barrier.getBlocks();
    auto &hpList = barrier.getBlockHp();
    for (int k = 0; k < (int)blocks.size(); k++) {
      if (hpList[k] <= 0)
        continue;
      if (bounds.intersects(blocks[k].getGlobalBounds())) {
        hpList[k]--;
        if (hpList[k] <= 0) {
          blocks[k].setSize(sf::Vector2f(0.f, 0.f)); // görünmez
        }
        return true;
      }
    }
  }
  return false;
}

// resetGame — tüm oyun değişkenlerini sıfırla
void GameManager::resetGame() {
  score = 0;
  lives = 3;
  level = 1;
  scoreText.setString("Skor: 0");
  livesText.setString("Can: 3");
  levelText.setString("Seviye: 1");

  swarmSpeed = 100.f;
  swarmDirection = 1;
  swarmMoveTimer = 0.f;
  swarmMoveInterval = 0.8f;
  dropPending = false;
  enemyShootTimer = 0.f;
  enemyShootInterval = 1.f;

  invincibleTimer = 0.f;
  blinkTimer = 0.f;
  playerVisible = true;

  bullets.clear();
  enemyBullets.clear();
  explosions.clear(); // Patlama particle sistemi
  kamikazeTimer_ = 0.f;
  kamikazeInterval_ = 8.f;

  // UFO sıfırla
  ufo_.active = false;
  sound_.stopUfo();
  ufoSpawnTimer_ = 0.f;
  ufoSpawnInterval_ = 20.f;

  barriers.clear();
  barriers.push_back(Barrier(100.f, BARRIER_Y));
  barriers.push_back(Barrier(350.f, BARRIER_Y));
  barriers.push_back(Barrier(600.f, BARRIER_Y));

  initLevel();
  gameState = State::Playing;
}

// initLevel — düşmanları oluştur (seviyeye göre zorluk artar)
void GameManager::initLevel() {
  enemies.clear();

  // her seviyede düşmanlar daha aşağıdan başlar
  float startY = 50.f + (level - 1) * 15.f;
  // Güvenlik sınırı: çok aşağı inmesin
  if (startY > 200.f)
    startY = 200.f;

  for (int i = 0; i < 5; i++) {
    EnemyType type;
    if (i == 0)
      type = EnemyType::A; // üst sıra: UFO
    else if (i <= 2)
      type = EnemyType::B; // orta 2 sıra: yengeç
    else
      type = EnemyType::C; // alt 2 sıra: böcek

    for (int j = 0; j < 8; j++)
      enemies.push_back(Enemy(50.f + j * 60.f, startY + i * 50.f, type));
  }
}

// run — ana oyun döngüsü
void GameManager::run() {
  sf::Clock clock;
  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();
    if (dt > 0.05f)
      dt = 0.05f;
    processEvents();
    update(dt);
    render();
  }
}

void GameManager::processEvents() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      window.close();

    if (event.type == sf::Event::KeyPressed) {
      // ESC her zaman kapatır
      if (event.key.code == sf::Keyboard::Escape)
        window.close();

      // M tuşu: Ses açma/kapama (her durumda çalışır)
      if (event.key.code == sf::Keyboard::M) {
        sound_.toggleMute();
        if (sound_.isMuted()) {
          soundStatusText.setFillColor(sf::Color(220, 120, 120));
          soundStatusText.setString("Ses: KAPALI  (M ile degistir)");
        } else {
          soundStatusText.setFillColor(sf::Color(120, 220, 120));
          soundStatusText.setString("Ses: ACIK  (M ile degistir)");
        }
        centreX(soundStatusText, WINDOW_WIDTH);
      }

      // P tuşu: Oyunu duraklat / devam ettir
      if (event.key.code == sf::Keyboard::P) {
        if (gameState == State::Playing)
          gameState = State::Paused;
        else if (gameState == State::Paused)
          gameState = State::Playing;
      }

      // Durum geçişleri
      // Menu     → Enter → Playing (resetGame ile)
      // GameOver → Enter → Playing
      if (event.key.code == sf::Keyboard::Enter) {
        if (gameState == State::Menu || gameState == State::GameOver) {
          resetGame();
        }
      }
    }
  }
}

void GameManager::update(float DeltaTime) {
  // Yıldızlar ve menü animasyonu her durumda çalışır (Playing, Menu, Paused,
  // GameOver)
  updateStars(DeltaTime);
  menuTimer_ += DeltaTime;

  // Menü ve bitiş ekranlarında oyun mantığı çalışmaz
  if (gameState != State::Playing)
    return;

  updateEntities(
      DeltaTime);    // 1. Obje pozisyonlarını ve zamanlayıcıları güncelle
  checkCollisions(); // 2. Çarpışmaları (Mermi, Oyuncu, Bariyer) kontrol et
  checkGameState(); // 3. Ölüleri temizle ve seviye atlama/bitiş şartlarını test
                    // et
}

void GameManager::updateEntities(float DeltaTime) {
  // Oyuncu ve Mermi Güncellemeleri
  int bulletsBefore = (int)bullets.size();
  player.update(DeltaTime, bullets);
  if ((int)bullets.size() > bulletsBefore)
    sound_.playShoot(); // Yeni mermi atıldıysa ses çal

  // Dokunulmazlık (Invincibility) süresi ve yanıp sönme efekti
  if (invincibleTimer > 0.f) {
    invincibleTimer -= DeltaTime;
    blinkTimer += DeltaTime;
    if (blinkTimer >= BLINK_RATE) {
      blinkTimer = 0.f;
      playerVisible = !playerVisible;
    }
    if (invincibleTimer <= 0.f) {
      invincibleTimer = 0.f;
      playerVisible = true;
    }
  }

  // Ekrandaki tüm mermileri ve patlamaları hareket ettir / zamanını ilerlet
  for (auto &b : bullets)
    b.update(DeltaTime);
  for (auto &eb : enemyBullets)
    eb.update(DeltaTime);
  for (auto &exp : explosions)
    exp.update(DeltaTime);

  // UFO (Mothership) Mantığı
  if (!ufo_.active) {
    ufoSpawnTimer_ += DeltaTime;
    if (ufoSpawnTimer_ >= ufoSpawnInterval_) {
      ufo_.active = true;
      ufoSpawnTimer_ = 0.f;
      // Bir sonraki çıkış 15-25 saniye sonra
      ufoSpawnInterval_ = 15.f + static_cast<float>(rand() % 11);
      // Rastgele yön seç: %50 soldan, %50 sağdan
      ufo_.direction = (rand() % 2 == 0) ? 1 : -1;
      ufo_.x = (ufo_.direction == 1) ? -50.f : WINDOW_WIDTH + 4.f;
      sound_.playUfo();
    }
  } else {
    ufo_.x += ufo_.speed * ufo_.direction * DeltaTime;
    // Ekran dışına çıktıysa pasifleştir
    if ((ufo_.direction == 1 && ufo_.x > WINDOW_WIDTH + 10.f) ||
        (ufo_.direction == -1 && ufo_.x < -56.f)) {
      ufo_.active = false;
      sound_.stopUfo();
    }
  }

  // Düşman Formasyonu (Swarm) Hareketi
  swarmMoveTimer += DeltaTime;
  if (swarmMoveTimer >= swarmMoveInterval) {
    swarmMoveTimer = 0.f;
    
    // YENİ: Düşmanlar adım attığında "kalp atışı" sesini çal
    sound_.playFleetStep();
    
    if (dropPending) {
      for (auto &e : enemies)
        if (!e.isKamikaze())
          e.move(0.f, dropDistance);
      swarmDirection *= -1;
      dropPending = false;
    } else {
      float step = swarmSpeed * swarmDirection * swarmMoveInterval;
      bool hitWall = false;
      for (auto &e : enemies) {
        if (e.isKamikaze())
          continue;
        float nx = e.getX() + step;
        if (nx <= LEFT_BOUND || nx >= RIGHT_BOUND) {
          hitWall = true;
          break;
        }
      }
      if (hitWall)
        dropPending = true;
      else
        for (auto &e : enemies)
          if (!e.isKamikaze())
            e.move(step, 0.f);
      float ratio = 1.f - (float)enemies.size() / 40.f;
      swarmMoveInterval = 0.8f * (1.f - ratio * 0.75f);
      if (swarmMoveInterval < 0.05f)
        swarmMoveInterval = 0.05f;
    }
  }

  // Düşman Ateşi
  enemyShootTimer += DeltaTime;
  if (enemyShootTimer >= enemyShootInterval && !enemies.empty()) {
    int idx = rand() % enemies.size();
    enemyBullets.push_back(Bullet(enemies[idx].getX() + 20.f,
                                  enemies[idx].getY() + 30.f, 1.f,
                                  sf::Color::Red));
    sound_.playEnemyShoot();
    enemyShootTimer = 0.f;
    enemyShootInterval = 0.5f + (float)rand() / (float)(RAND_MAX / 1.5f);
  }

  // Kamikaze Sistemi (Belirli aralıklarla düşmanın oyuncuya dalması)
  kamikazeTimer_ += DeltaTime;
  if (kamikazeTimer_ >= kamikazeInterval_) {
    kamikazeTimer_ = 0.f;
    std::vector<int> candidates;
    for (int i = 0; i < (int)enemies.size(); i++) {
      if (enemies[i].getType() == EnemyType::C && !enemies[i].isKamikaze())
        candidates.push_back(i);
    }
    if (!candidates.empty()) {
      int pick = candidates[rand() % candidates.size()];
      sf::FloatRect pb = player.getBounds();
      float tx = pb.left + pb.width / 2.f;
      float ty = pb.top + pb.height / 2.f;
      enemies[pick].activateKamikaze(tx, ty);
    }
    kamikazeInterval_ = std::max(3.f, 8.f - level * 0.8f);
  }

  // Kamikaze moduna geçmiş düşmanların pozisyon güncellemesi
  for (auto &e : enemies) {
    if (e.isKamikaze())
      e.updateKamikaze(DeltaTime);
  }
}

void GameManager::checkCollisions() {
  // Oyuncu Mermileri vs Düşmanlar ve Bariyerler
  std::vector<bool> bulletAlive(bullets.size(), true);
  for (int i = 0; i < (int)bullets.size(); i++) {
    if (bullets[i].getY() < 0) {
      bulletAlive[i] = false;
      continue;
    }
    bool hit = false;

    // UFO Çarpışması
    if (ufo_.active && bullets[i].getBounds().intersects(ufo_.getBounds())) {
      ufo_.active = false;
      sound_.stopUfo();

      // Orijinal oyun: 50, 100, 150 veya 300 puan (rastgele)
      static const int UFO_POINTS[] = {50, 100, 150, 300};
      int pts = UFO_POINTS[rand() % 4];
      score += pts;
      scoreText.setString("Skor: " + std::to_string(score));

      explosions.emplace_back(ufo_.x + 23.f, ufo_.y + 10.f,
                              sf::Color(255, 60, 60), 18);
      sound_.playExplosion();
      bulletAlive[i] = false;
      continue;
    }

    for (auto &enemy : enemies) {
      if (!enemy.isAlive())
        continue;
      if (bullets[i].getBounds().intersects(enemy.getBounds())) {
        enemy.takeDamage(1);
        score += enemy.getScore();
        scoreText.setString("Skor: " + std::to_string(score));
        sf::Color expColor;
        switch (enemy.getType()) {
        case EnemyType::A:
          expColor = sf::Color(255, 80, 220);
          break;
        case EnemyType::B:
          expColor = sf::Color(255, 160, 40);
          break;
        case EnemyType::C:
          expColor = sf::Color(255, 80, 80);
          break;
        }
        explosions.emplace_back(enemy.getX() + 20.f, enemy.getY() + 15.f,
                                expColor, 14);
        sound_.playExplosion();
        hit = true;
        break;
      }
    }
    if (hit) {
      bulletAlive[i] = false;
      continue;
    }

    if (checkBarrierCollision(bullets[i].getBounds())) {
      bulletAlive[i] = false;
      hit = true;
    }
  }
  {
    int idx = 0;
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [&](const Bullet &) { return !bulletAlive[idx++]; }),
        bullets.end());
  }

  // Düşman Mermileri vs Oyuncu ve Bariyerler
  std::vector<bool> eBulletAlive(enemyBullets.size(), true);
  for (int i = 0; i < (int)enemyBullets.size(); i++) {
    if (enemyBullets[i].getY() > WINDOW_HEIGHT) {
      eBulletAlive[i] = false;
      continue;
    }

    if (invincibleTimer <= 0.f &&
        enemyBullets[i].getBounds().intersects(player.getBounds())) {
      eBulletAlive[i] = false;
      playerHit(false);
      continue;
    }
    bool hit = false;
    if (checkBarrierCollision(enemyBullets[i].getBounds())) {
      eBulletAlive[i] = false;
      hit = true;
    }
  }
  {
    int idx = 0;
    enemyBullets.erase(
        std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                       [&](const Bullet &) { return !eBulletAlive[idx++]; }),
        enemyBullets.end());
  }

  // Kamikaze Düşman vs Oyuncu Çarpışması
  for (auto &e : enemies) {
    if (!e.isKamikaze())
      continue;
    if (invincibleTimer <= 0.f &&
        e.getBounds().intersects(player.getBounds())) {
      e.takeDamage(1);
      playerHit(true);
    }
    if (e.hasReachedTarget())
      e.takeDamage(1);
  }
}

void GameManager::checkGameState() {
  // Ölü düşmanları ve biten patlama efektlerini listeden temizle
  enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                               [](const Enemy &e) { return !e.isAlive(); }),
                enemies.end());

  explosions.erase(
      std::remove_if(explosions.begin(), explosions.end(),
                     [](const Explosion &e) { return e.isFinished(); }),
      explosions.end());

  // Düşmanların ekranın en altına inip inmediğini kontrol et (Game Over şartı)
  if (gameState == State::Playing) {
    for (auto &enemy : enemies) {
      if (!enemy.isKamikaze() && enemy.getY() + 30.f >= ENEMY_BOTTOM_LIMIT) {
        gameState = State::GameOver;
        break;
      }
    }
  }

  // Bölüm sonu — Sürekli yeni dalga (sonsuz seviye)
  if (enemies.empty() && gameState == State::Playing) {
    level++;
    levelText.setString("Seviye: " + std::to_string(level));
    swarmSpeed += 15.f;
    swarmMoveInterval = 0.8f;
    dropPending = false;
    swarmDirection = 1;
    enemyShootInterval = std::max(0.25f, enemyShootInterval * 0.85f);
    kamikazeInterval_ = std::max(2.5f, 8.f - level * 0.6f);
    bullets.clear();
    enemyBullets.clear();

    barriers.clear();
    barriers.push_back(Barrier(100.f, BARRIER_Y));
    barriers.push_back(Barrier(350.f, BARRIER_Y));
    barriers.push_back(Barrier(600.f, BARRIER_Y));

    // High score güncelle
    if (score > highScore) {
      highScore = score;
      highScoreText.setString("En Yuksek Skor: " + std::to_string(highScore));
      centreX(highScoreText, WINDOW_WIDTH);
      saveHighScore();
    }

    initLevel();
  }
}

void GameManager::render() {
  window.clear(sf::Color(5, 5, 15)); // koyu lacivert arka plan
  drawStars();                       // yıldızlar her ekranda çizilir

  if (gameState == State::Menu) {
    // Animasyonlu başlık
    float pulse = (std::sin(menuTimer_ * 2.5f) + 1.f) / 2.f;
    float titleY = 80.f + std::sin(menuTimer_ * 1.5f) * 6.f;
    sf::Uint8 r = static_cast<sf::Uint8>(80 + pulse * 175);
    sf::Uint8 g = static_cast<sf::Uint8>(200 + pulse * 55);
    menuTitleText.setFillColor(sf::Color(r, g, 255));
    menuTitleText.setPosition(menuTitleText.getPosition().x, titleY);
    window.draw(menuTitleText);

    // Alt başlık
    sf::Text subtitle;
    subtitle.setFont(font);
    subtitle.setCharacterSize(14);
    subtitle.setFillColor(sf::Color(100, 160, 220));
    subtitle.setString("~ Galaksiyi istilacillardan kurtar ~");
    centreX(subtitle, WINDOW_WIDTH);
    subtitle.setPosition(subtitle.getPosition().x, 165.f);
    window.draw(subtitle);

    // High Score
    highScoreText.setPosition(highScoreText.getPosition().x, 210.f);
    window.draw(highScoreText);

    // Kontrol bilgisi
    menuSubText.setPosition(menuSubText.getPosition().x, 260.f);
    window.draw(menuSubText);

    // Düşman puan tablosu
    sf::Text tableTitle;
    tableTitle.setFont(font);
    tableTitle.setCharacterSize(16);
    tableTitle.setFillColor(sf::Color(180, 220, 255));
    tableTitle.setString("---PUAN TABLOSU --");
    centreX(tableTitle, WINDOW_WIDTH);
    tableTitle.setPosition(tableTitle.getPosition().x, 310.f);
    window.draw(tableTitle);

    struct EnemyInfo {
      const char *name;
      const char *pts;
      sf::Color color;
    };
    EnemyInfo infos[] = {{"Pembe", "30", sf::Color(255, 80, 220)},
                         {"Turuncu", "20", sf::Color(255, 160, 40)},
                         {"Kirmizi", "10", sf::Color(255, 80, 80)}};
    float infoY = 340.f;
    for (int i = 0; i < 3; i++) {
      // Renkli düşman ikonu
      sf::RectangleShape icon(sf::Vector2f(18.f, 14.f));
      icon.setFillColor(infos[i].color);
      icon.setOutlineColor(sf::Color(255, 255, 255, 60));
      icon.setOutlineThickness(1.f);
      icon.setPosition(280.f, infoY + i * 32.f + 2.f);
      window.draw(icon);

      sf::Text nameText;
      nameText.setFont(font);
      nameText.setCharacterSize(17);
      nameText.setFillColor(infos[i].color);
      nameText.setString(infos[i].name);
      nameText.setPosition(308.f, infoY + i * 32.f);
      window.draw(nameText);

      sf::Text ptsText;
      ptsText.setFont(font);
      ptsText.setCharacterSize(17);
      ptsText.setFillColor(sf::Color(220, 220, 220));
      ptsText.setString(std::string("... ") + infos[i].pts + " puan");
      ptsText.setPosition(400.f, infoY + i * 32.f);
      window.draw(ptsText);
    }

    // Kontrol kısayolları
    sf::Text controls;
    controls.setFont(font);
    controls.setCharacterSize(15);
    controls.setFillColor(sf::Color(100, 100, 120));
    controls.setString("P: Duraklat     ESC: Cikis");
    centreX(controls, WINDOW_WIDTH);
    controls.setPosition(controls.getPosition().x, 448.f);
    window.draw(controls);

    // Yanıp sönen ENTER ipucu
    float blinkAlpha = (std::sin(menuTimer_ * 4.f) + 1.f) / 2.f;
    sf::Text enterHint;
    enterHint.setFont(font);
    enterHint.setCharacterSize(26);
    enterHint.setFillColor(sf::Color(
        255, 255, 100, static_cast<sf::Uint8>(80 + blinkAlpha * 175)));
    enterHint.setString("[  ENTER ile baslat  ]");
    centreX(enterHint, WINDOW_WIDTH);
    enterHint.setPosition(enterHint.getPosition().x, 490.f);
    window.draw(enterHint);

    // Ses durumu
    window.draw(soundStatusText);

  } else if (gameState == State::Playing) {
    for (auto &enemy : enemies)
      enemy.draw(window);
    for (auto &bullet : bullets)
      bullet.draw(window);
    for (auto &eBullet : enemyBullets)
      eBullet.draw(window);
    for (auto &barrier : barriers)
      barrier.draw(window);
    if (playerVisible)
      player.draw(window);
    for (auto &exp : explosions)
      exp.draw(window);
    ufo_.draw(window); // UFO en üste çizilir

    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);

  } else if (gameState == State::GameOver) {
    // High score kontrolü
    if (score > highScore) {
      highScore = score;
      highScoreText.setString("En Yuksek Skor: " + std::to_string(highScore));
      centreX(highScoreText, WINDOW_WIDTH);
      saveHighScore();
    }

    window.draw(gameOverText);

    // Skor ve seviye bilgisi
    sf::Text finalScore;
    finalScore.setFont(font);
    finalScore.setCharacterSize(28);
    finalScore.setFillColor(sf::Color::White);
    finalScore.setString("Skor: " + std::to_string(score) +
                         "     Seviye: " + std::to_string(level));
    centreX(finalScore, WINDOW_WIDTH);
    finalScore.setPosition(finalScore.getPosition().x, 290.f);
    window.draw(finalScore);

    // High score
    window.draw(highScoreText);
    highScoreText.setPosition(highScoreText.getPosition().x, 340.f);
    window.draw(restartHintText);

  } else if (gameState == State::Paused) {
    // Pause ekranında oyun sahnesini de çiz (donmuş halde)
    for (auto &enemy : enemies)
      enemy.draw(window);
    for (auto &bullet : bullets)
      bullet.draw(window);
    for (auto &eBullet : enemyBullets)
      eBullet.draw(window);
    for (auto &barrier : barriers)
      barrier.draw(window);
    if (playerVisible)
      player.draw(window);
    for (auto &exp : explosions)
      exp.draw(window);
    ufo_.draw(window); // Pause'da da UFO görünsün
    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);

    // Yarı saydam karartma overlay
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // Pause metinleri
    window.draw(pauseText);
    window.draw(pauseHintText);
    window.draw(soundStatusText);
  }

  window.display();
}

void GameManager::loadHighScore() {
  std::ifstream file("highscore.txt");
  if (file.is_open()) {
    file >> highScore;
    file.close();
  } else {
    highScore = 0;
  }
}

void GameManager::saveHighScore() {
  std::ofstream file("highscore.txt");
  if (file.is_open()) {
    file << highScore;
    file.close();
  }
}

// ── Parallax Yıldız Sistemi ──────────────────────────────────────────────
void GameManager::initStars() {
  stars_.clear();
  stars_.reserve(STAR_LAYERS * STARS_PER_LAYER);
  for (int layer = 0; layer < STAR_LAYERS; layer++) {
    for (int i = 0; i < STARS_PER_LAYER; i++) {
      Star s;
      s.x = static_cast<float>(rand() % static_cast<int>(WINDOW_WIDTH));
      s.y = static_cast<float>(rand() % static_cast<int>(WINDOW_HEIGHT));
      // Katman 0: uzak (yavaş, küçük, soluk)
      // Katman 2: yakın (hızlı, büyük, parlak)
      s.speed = 15.f + layer * 22.f;
      s.size = 0.8f + layer * 0.7f;
      s.brightness = static_cast<sf::Uint8>(55 + layer * 75);
      stars_.push_back(s);
    }
  }
}

void GameManager::updateStars(float dt) {
  for (auto &s : stars_) {
    s.y += s.speed * dt;
    if (s.y > WINDOW_HEIGHT) {
      s.y = 0.f;
      s.x = static_cast<float>(rand() % static_cast<int>(WINDOW_WIDTH));
    }
  }
}

void GameManager::drawStars() {
  for (const auto &s : stars_) {
    sf::CircleShape dot(s.size);
    // Mavi tonlu beyaz
    dot.setFillColor(
        sf::Color(s.brightness, s.brightness,
                  static_cast<sf::Uint8>(std::min(255, s.brightness + 40))));
    dot.setPosition(s.x, s.y);
    window.draw(dot);
  }
}