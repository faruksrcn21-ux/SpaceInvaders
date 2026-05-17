#include "GameManager.h"
#include "Enemy.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>

// Yardımcı: metni yatayda ortala
void GameManager::centreX(sf::Text &t, float windowWidth) {
  sf::FloatRect r = t.getLocalBounds();
  t.setOrigin(r.left + r.width / 2.f, 0.f);
  t.setPosition(windowWidth / 2.f, t.getPosition().y);
}

GameManager::GameManager()
    : window(sf::VideoMode((unsigned int)WINDOW_WIDTH,
                           (unsigned int)WINDOW_HEIGHT),
             "Space Invaders") {
  // Kaliteli random seed (time(0) yerine donanım entropi kaynağı)
  srand(std::random_device{}());

  // Font yükleme — birden fazla platform yolu denenir
  static const char *FONT_PATHS[] = {
      "assets/font.ttf",
      "C:/Windows/Fonts/consola.ttf",
      "C:/Windows/Fonts/arial.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
      "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
      "/Library/Fonts/Courier New.ttf"};
  bool fontLoaded = false;
  for (const char *path : FONT_PATHS) {
    if (font.loadFromFile(path)) {
      fontLoaded = true;
      break;
    }
  }
  if (!fontLoaded)
    std::cerr << "[UYARI] Font yuklenemedi! 'assets/font.ttf' ekleyin.\n";

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
  menuSubText.setString("Ok tuslari: Hareket     Bosluk: Ates     X: Bomba");
  centreX(menuSubText, WINDOW_WIDTH);
  menuSubText.setPosition(menuSubText.getPosition().x, 290.f);

  // Ortak "yeniden başlat" ipucu
  restartHintText.setFont(font);
  restartHintText.setCharacterSize(24);
  restartHintText.setFillColor(sf::Color(180, 180, 180));
  restartHintText.setString("ENTER veya R: Tekrar oyna     ESC: Cikis");
  centreX(restartHintText, WINDOW_WIDTH);
  restartHintText.setPosition(restartHintText.getPosition().x, 430.f);

  // Pause ekranı metinleri
  pauseText.setFont(font);
  pauseText.setCharacterSize(64);
  pauseText.setFillColor(sf::Color::Yellow);
  pauseText.setString("DURAKLATILDI");
  centreX(pauseText, WINDOW_WIDTH);
  pauseText.setPosition(pauseText.getPosition().x, 200.f);

  pauseHintText.setFont(font);
  pauseHintText.setCharacterSize(22);
  pauseHintText.setFillColor(sf::Color(200, 200, 200));
  pauseHintText.setString("P: Devam et     R: Reset     ESC: Cikis");
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
  shockwaveRadius_ = 400.f; // Başlangıçta çizilmesin
  bombAmmo_ = 0;            // Bomba cephanesini ilklendir

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
  // Eğer kalkan aktifse, darbeyi kalkan emsin!
  if (shieldTimer_ > 0.f && shieldHealth_ > 0) {
    shieldHealth_--;
    if (shieldHealth_ <= 0) {
      shieldTimer_ = 0.f; // kalkan kırıldı
    }

    // Kalkan darbe emme görsel efekti
    sf::FloatRect pb = player.getBounds();
    explosions.emplace_back(pb.left + pb.width / 2.f, pb.top + pb.height / 2.f,
                            sf::Color(80, 200, 255),
                            12); // Parlak mavi kıvılcımlar
    sound_.playExplosion();
    addScreenShake(0.2f, 6.f);

    addFloatingText(player.getPosition().x - 30.f, player.getPosition().y - 45.f, "KALKAN KIRILDI!", sf::Color(80, 200, 255), 15);
    return;
  }

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
  addScreenShake(0.5f, 15.f); // Oyuncu vurulunca şiddetli sarsıntı

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

        // Siper parçacıkları (Red/Orange sparks matching the red barrier)
        float bx = blocks[k].getPosition().x + 2.5f;
        float by = blocks[k].getPosition().y + 2.5f;
        explosions.emplace_back(bx, by, sf::Color(220, 60, 0), 3);

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
  floatingTexts_.clear();
  thrustParticles_.clear(); // İtki parçacıklarını temizle
  shakeTimer_ = 0.f;
  kamikazeTimer_ = 0.f;
  kamikazeInterval_ = 8.f;
  levelUpTimer_ = 0.f; // LevelUp sayacı
  newRecord_ = false;  // Yeni Rekor kutlaması

  // Power-up Sistemini Sıfırla
  shieldTimer_ = 0.f;
  tripleShotTimer_ = 0.f;
  rapidFireTimer_ = 0.f;
  shieldHealth_ = 0;
  bombAmmo_ = 0;            // Bomba cephanesini sıfırla
  shockwaveRadius_ = 400.f; // Başlangıçta çizilmesin
  powerUps_.clear();

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
      if (event.key.code == sf::Keyboard::Enter ||
          event.key.code == sf::Keyboard::R) {
        if (gameState == State::Menu || gameState == State::GameOver) {
          resetGame();
        }
      }

      // Oyun esnasında veya Pause'da R tuşu ile direkt reset
      if (event.key.code == sf::Keyboard::R &&
          (gameState == State::Playing || gameState == State::Paused)) {
        resetGame();
      }
    }
  }
}

void GameManager::update(float DeltaTime) {
  // Yıldızlar ve menü animasyonu her durumda çalışır (Playing, Menu, Paused,
  // GameOver)
  updateStars(DeltaTime);
  menuTimer_ += DeltaTime;

  // Ekran sarsıntısını güncelle
  if (shakeTimer_ > 0.f) {
    shakeTimer_ -= DeltaTime;
    if (shakeTimer_ < 0.f)
      shakeTimer_ = 0.f;
  }

  // LevelUp sayacı — 2sn sonra yeni dalgayı başlat
  if (gameState == State::LevelUp) {
    levelUpTimer_ -= DeltaTime;
    if (levelUpTimer_ <= 0.f) {
      // Yeni dalgayı hazırla
      swarmSpeed = std::min(swarmSpeed + 10.f, 250.f); // Sınırlı artış
      swarmMoveInterval = std::max(0.5f, swarmMoveInterval * 0.95f);
      dropPending = false;
      swarmDirection = 1;
      enemyShootInterval = std::max(0.2f, enemyShootInterval * 0.9f);
      kamikazeInterval_ = std::max(3.f, 8.f - level * 0.4f);

      // Vektörleri güvenli bir şekilde temizle
      bullets.clear();
      enemyBullets.clear();
      explosions.clear();
      floatingTexts_.clear();
      thrustParticles_.clear(); // İtki parçacıklarını temizle
      barriers.clear();
      barriers.push_back(Barrier(100.f, BARRIER_Y));
      barriers.push_back(Barrier(350.f, BARRIER_Y));
      barriers.push_back(Barrier(600.f, BARRIER_Y));
      initLevel();
      gameState = State::Playing;
    }
    return; // LevelUp ekranındayken oyun mantığı çalışmasın
  }

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
  bool rapidActive = (rapidFireTimer_ > 0.f);
  bool tripleActive = (tripleShotTimer_ > 0.f);
  player.update(DeltaTime, bullets, rapidActive, tripleActive, bombAmmo_);
  if ((int)bullets.size() > bulletsBefore)
    sound_.playShoot(); // Yeni mermi atıldıysa ses çal

  // Oyuncu Motor Alevi Efekti (Thrust Particles)
  if (gameState == State::Playing) {
    bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    if (left || right) {
      sf::Vector2f playerPos = player.getPosition();
      for (int side = 0; side < 2; side++) {
        float ex = (side == 0) ? (playerPos.x - 11.5f) : (playerPos.x + 11.5f);
        float ey = playerPos.y + 8.f;

        ThrustParticle p;
        float sz = 2.f + static_cast<float>(rand() % 3);
        p.shape.setSize(sf::Vector2f(sz, sz));
        p.shape.setOrigin(sz / 2.f, sz / 2.f);
        p.shape.setPosition(ex, ey);

        int colType = rand() % 3;
        if (colType == 0)
          p.shape.setFillColor(sf::Color(255, 100, 0));
        else if (colType == 1)
          p.shape.setFillColor(sf::Color(255, 200, 0));
        else
          p.shape.setFillColor(sf::Color(255, 50, 0));

        float vx = -20.f + static_cast<float>(rand() % 41);
        float vy = 80.f + static_cast<float>(rand() % 61);
        p.velocity = sf::Vector2f(vx, vy);

        p.maxLifetime = 0.15f + static_cast<float>(rand() % 16) / 100.f;
        p.lifetime = p.maxLifetime;

        thrustParticles_.push_back(p);
      }
    }
  }

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

  // İtki parçacıklarını güncelle ve temizle
  for (auto &p : thrustParticles_) {
    p.lifetime -= DeltaTime;
    p.shape.move(p.velocity * DeltaTime);
    p.velocity.y *= (1.f - 1.f * DeltaTime);
    float ratio = std::max(0.f, p.lifetime / p.maxLifetime);
    sf::Color c = p.shape.getFillColor();
    c.a = static_cast<sf::Uint8>(255.f * ratio);
    p.shape.setFillColor(c);
  }
  thrustParticles_.erase(
      std::remove_if(thrustParticles_.begin(), thrustParticles_.end(),
                     [](const ThrustParticle &p) { return p.lifetime <= 0.f; }),
      thrustParticles_.end());

  // Yüzen yazıları güncelle
  for (auto &ft : floatingTexts_)
    ft.update(DeltaTime);
  floatingTexts_.erase(std::remove_if(floatingTexts_.begin(),
                                      floatingTexts_.end(),
                                      [](const FloatingText &ft) {
                                        return ft.lifeTimer <= 0.f;
                                      }),
                       floatingTexts_.end());

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

  // Power-up Sayaçlarını Azalt
  if (shieldTimer_ > 0.f) {
    shieldTimer_ -= DeltaTime;
    if (shieldTimer_ <= 0.f) {
      shieldTimer_ = 0.f;
      shieldHealth_ = 0;
    }
  }
  if (tripleShotTimer_ > 0.f) {
    tripleShotTimer_ -= DeltaTime;
    if (tripleShotTimer_ <= 0.f)
      tripleShotTimer_ = 0.f;
  }
  if (rapidFireTimer_ > 0.f) {
    rapidFireTimer_ -= DeltaTime;
    if (rapidFireTimer_ <= 0.f)
      rapidFireTimer_ = 0.f;
  }

  // Düşen Kapsülleri (Power-ups) Güncelle
  for (auto &p : powerUps_) {
    p.pos.y += p.speed * DeltaTime;
    p.rotation += 140.f * DeltaTime; // Kapsülün dönme animasyonu
  }
  // Ekran dışına çıkan kapsülleri sil
  powerUps_.erase(std::remove_if(powerUps_.begin(), powerUps_.end(),
                                 [](const PowerUp &p) {
                                   return p.pos.y > WINDOW_HEIGHT + 20.f;
                                 }),
                  powerUps_.end());

  // Şok Dalgasını (Shockwave Halkası) Güncelle
  if (shockwaveRadius_ < shockwaveMaxRadius_) {
    shockwaveRadius_ += 800.f * DeltaTime; // Hızlıca dışa yayılan halka
  }

  // Düşman Formasyonu (Swarm) Hareketi
  swarmMoveTimer += DeltaTime;
  if (swarmMoveTimer >= swarmMoveInterval) {
    swarmMoveTimer = 0.f;

    // Düşmanlar adım attığında "kalp atışı" sesini çal
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
      if (bullets[i].isBomb()) {
        detonateBomb(bullets[i].getBounds().left + 8.f, 10.f);
      }
      bulletAlive[i] = false;
      continue;
    }
    bool hit = false;

    // UFO Çarpışması
    if (ufo_.active && bullets[i].getBounds().intersects(ufo_.getBounds())) {
      if (bullets[i].isBomb()) {
        detonateBomb(bullets[i].getBounds().left + 8.f,
                     bullets[i].getBounds().top + 8.f);
      } else {
        ufo_.active = false;
        sound_.stopUfo();
        spawnPowerUp(ufo_.x + 23.f, ufo_.y + 10.f, 0.50f); // %50 olasılık

        // 50, 100, 150 veya 300 puan (rastgele)
        static const int UFO_POINTS[] = {50, 100, 150, 300};
        int pts = UFO_POINTS[rand() % 4];
        score += pts;
        scoreText.setString("Skor: " + std::to_string(score));

        // Yüzen puan yazısı
        addFloatingText(ufo_.x + 10.f, ufo_.y - 10.f, "+" + std::to_string(pts), sf::Color::Cyan, 20, 1.2f, 40.f);

        explosions.emplace_back(ufo_.x + 23.f, ufo_.y + 10.f,
                                sf::Color(255, 60, 60), 18);
        sound_.playExplosion();
        addScreenShake(0.3f, 8.f); // UFO patlayınca hafif sarsıntı
      }
      bulletAlive[i] = false;
      continue;
    }

    for (auto &enemy : enemies) {
      if (!enemy.isAlive())
        continue;
      if (bullets[i].getBounds().intersects(enemy.getBounds())) {
        if (bullets[i].isBomb()) {
          detonateBomb(bullets[i].getBounds().left + 8.f,
                       bullets[i].getBounds().top + 8.f);
        } else {
          enemy.takeDamage(1);
          if (!enemy.isAlive()) {
            float dropChance =
                enemy.isKamikaze() ? 0.15f : 0.05f; // Kamikaze %15, Normal %5
            spawnPowerUp(enemy.getX() + 20.f, enemy.getY() + 15.f, dropChance);
          }
          int pts = enemy.getScore();
          score += pts;
          scoreText.setString("Skor: " + std::to_string(score));

          // Yüzen puan yazısı
          addFloatingText(enemy.getX() + 10.f, enemy.getY() - 10.f, "+" + std::to_string(pts), sf::Color::Yellow, 14, 0.8f, 30.f);

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
        }
        hit = true;
        break;
      }
    }
    if (hit) {
      bulletAlive[i] = false;
      continue;
    }

    if (checkBarrierCollision(bullets[i].getBounds())) {
      if (bullets[i].isBomb()) {
        detonateBomb(bullets[i].getBounds().left + 8.f,
                     bullets[i].getBounds().top + 8.f);
      }
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

    if (checkBarrierCollision(enemyBullets[i].getBounds())) {
      eBulletAlive[i] = false;
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

  // Düşman Gemileri vs Bariyerler (Kemirme Efekti)
  for (auto &enemy : enemies) {
    if (!enemy.isAlive())
      continue;
    // Optimizasyon: Düşman siper hizasında değilse kontrol etme
    if (enemy.getY() + 30.f < BARRIER_Y)
      continue;

    sf::FloatRect eb = enemy.getBounds();
    bool collidedWithBarrier = false;
    for (auto &barrier : barriers) {
      auto &blocks = barrier.getBlocks();
      auto &hpList = barrier.getBlockHp();
      for (int k = 0; k < (int)blocks.size(); k++) {
        if (hpList[k] > 0 && eb.intersects(blocks[k].getGlobalBounds())) {
          hpList[k] = 0; // Anında yok et
          blocks[k].setSize(sf::Vector2f(0.f, 0.f));

          float bx = blocks[k].getPosition().x + 2.5f;
          float by = blocks[k].getPosition().y + 2.5f;
          explosions.emplace_back(bx, by, sf::Color(220, 60, 0),
                                  2); // Kırmızı siper parçacığı

          collidedWithBarrier = true;
        }
      }
    }

    // Kamikaze sipere çarptıysa patlasın ve ölsün
    if (collidedWithBarrier && enemy.isKamikaze()) {
      enemy.takeDamage(1);
      explosions.emplace_back(enemy.getX() + 20.f, enemy.getY() + 15.f,
                              sf::Color(255, 100, 100), 10);
      sound_.playExplosion();
      addScreenShake(0.2f, 5.f);
    }
  }

  // Oyuncu vs Güçlendirme Kapsülleri Çarpışması
  sf::FloatRect pb = player.getBounds();
  std::vector<bool> pAlive(powerUps_.size(), true);
  for (int i = 0; i < (int)powerUps_.size(); i++) {
    if (pb.intersects(powerUps_[i].getBounds())) {
      pAlive[i] = false;

      std::string floatingMsg = "";
      sf::Color textCol = sf::Color::White;

      static constexpr float POWERUP_DURATION = 7.0f;
      switch (powerUps_[i].type) {
      case PowerUpType::Shield:
        shieldTimer_ = POWERUP_DURATION;
        shieldHealth_ = 1;
        floatingMsg = "+KALKAN";
        textCol = sf::Color(80, 200, 255);
        sound_.playPowerupPickup();
        break;
      case PowerUpType::TripleShot:
        tripleShotTimer_ = POWERUP_DURATION;
        floatingMsg = "+UCLU ATIS";
        textCol = sf::Color(255, 140, 0);
        sound_.playPowerupPickup();
        break;
      case PowerUpType::RapidFire:
        rapidFireTimer_ = POWERUP_DURATION;
        floatingMsg = "+HIZLI ATIS";
        textCol = sf::Color(255, 220, 0);
        sound_.playPowerupPickup();
        break;
      case PowerUpType::Bomb:
        floatingMsg = "+BOMBA CEPHANESI";
        textCol = sf::Color(255, 0, 200);
        bombAmmo_++;                // Bomba cephanesini arttır
        sound_.playPowerupPickup(); // Sentezlenen retro sweep sesini çal
        break;
      }

      // Yüzen bilgi yazısını tetikle
      addFloatingText(player.getPosition().x - 20.f, player.getPosition().y - 40.f, floatingMsg, textCol, 16, 1.0f, 40.f);
    }
  }
  {
    int idx = 0;
    powerUps_.erase(
        std::remove_if(powerUps_.begin(), powerUps_.end(),
                       [&](const PowerUp &) { return !pAlive[idx++]; }),
        powerUps_.end());
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
        // High score kontrolü — state geçişinde yapılır, render'da değil
        if (score > highScore) {
          highScore = score;
          newRecord_ = true;
          highScoreText.setString("En Yuksek Skor: " +
                                  std::to_string(highScore));
          centreX(highScoreText, WINDOW_WIDTH);
          saveHighScore();
        }
        gameState = State::GameOver;
        break;
      }
    }
  }

  // Bölüm sonu — seviye geçiş ekranı göster, sonra yeni dalga
  if (enemies.empty() && gameState == State::Playing) {
    level++;
    levelText.setString("Seviye: " + std::to_string(level));

    // Yeni rekor kontrolü
    if (score > highScore) {
      highScore = score;
      newRecord_ = true;
      highScoreText.setString("En Yuksek Skor: " + std::to_string(highScore));
      centreX(highScoreText, WINDOW_WIDTH);
      saveHighScore();
    }

    // Playing → LevelUp, 2sn sonra yeni dalga başlar
    levelUpTimer_ = LEVELUP_DURATION;
    gameState = State::LevelUp;
  }
}

void GameManager::render() {
  window.clear(sf::Color(5, 5, 15)); // koyu lacivert arka plan

  // Ekran Sarsıntısı (Screen Shake) Uygulama
  sf::View view = window.getDefaultView();
  if (shakeTimer_ > 0.f) {
    float offsetX =
        ((rand() % 100) / 50.f - 1.f) * shakeMagnitude_ * (shakeTimer_ / 0.5f);
    float offsetY =
        ((rand() % 100) / 50.f - 1.f) * shakeMagnitude_ * (shakeTimer_ / 0.5f);
    view.move(offsetX, offsetY);
  }
  window.setView(view);

  drawStars(); // yıldızlar her ekranda çizilir

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

      nameText.setFont(font);
      nameText.setCharacterSize(17);
      nameText.setFillColor(infos[i].color);
      nameText.setString(infos[i].name);
      nameText.setPosition(308.f, infoY + i * 32.f);
      window.draw(nameText);

      ptsText.setFont(font);
      ptsText.setCharacterSize(17);
      ptsText.setFillColor(sf::Color(220, 220, 220));
      ptsText.setString(std::string("... ") + infos[i].pts + " puan");
      ptsText.setPosition(400.f, infoY + i * 32.f);
      window.draw(ptsText);
    }

    // Kontrol kısayolları
    controlsText.setFont(font);
    controlsText.setCharacterSize(15);
    controlsText.setFillColor(sf::Color(100, 100, 120));
    controlsText.setString("P: Duraklat     ESC: Cikis");
    centreX(controlsText, WINDOW_WIDTH);
    controlsText.setPosition(controlsText.getPosition().x, 448.f);
    window.draw(controlsText);

    // Yanıp sönen ENTER ipucu
    float blinkAlpha = (std::sin(menuTimer_ * 4.f) + 1.f) / 2.f;
    enterHintText.setFont(font);
    enterHintText.setCharacterSize(26);
    enterHintText.setFillColor(sf::Color(
        255, 255, 100, static_cast<sf::Uint8>(80 + blinkAlpha * 175)));
    enterHintText.setString("[  ENTER ile baslat  ]");
    centreX(enterHintText, WINDOW_WIDTH);
    enterHintText.setPosition(enterHintText.getPosition().x, 490.f);
    window.draw(enterHintText);

    // Ses durumu
    window.draw(soundStatusText);

  } else if (gameState == State::Playing) {
    // Güvenli vector rendering
    if (!enemies.empty())
      for (auto &enemy : enemies)
        enemy.draw(window);
    if (!bullets.empty())
      for (auto &bullet : bullets)
        bullet.draw(window);
    if (!enemyBullets.empty())
      for (auto &eBullet : enemyBullets)
        eBullet.draw(window);
    if (!barriers.empty())
      for (auto &barrier : barriers)
        barrier.draw(window);

    // Motor itki parçacıklarını çiz
    for (const auto &p : thrustParticles_) {
      window.draw(p.shape);
    }

    if (playerVisible) {
      player.draw(window);

      // Oyuncu Kalkan Halkasını Çiz
      if (shieldTimer_ > 0.f && shieldHealth_ > 0) {
        sf::CircleShape shieldShape(34.f);
        shieldShape.setOrigin(34.f, 34.f);
        shieldShape.setPosition(player.getPosition().x,
                                player.getPosition().y - 12.f);

        // Zayıflarken yanıp sönme efekti
        sf::Uint8 alpha = 80;
        if (shieldTimer_ < 2.f) {
          alpha = static_cast<sf::Uint8>(
              40 + 70 * (static_cast<int>(shieldTimer_ * 8.f) % 2));
        }

        shieldShape.setFillColor(sf::Color(80, 200, 255, alpha));
        shieldShape.setOutlineColor(
            sf::Color(150, 230, 255, static_cast<sf::Uint8>(alpha * 2)));
        shieldShape.setOutlineThickness(2.f);
        window.draw(shieldShape);
      }
    }

    // Düşen Kapsülleri (Power-ups) Çiz
    for (const auto &p : powerUps_) {
      sf::RectangleShape capShape(sf::Vector2f(13.f, 20.f));
      capShape.setOrigin(6.5f, 10.f);
      capShape.setPosition(p.pos);
      capShape.setRotation(p.rotation);

      sf::Color c;
      std::string l = "";
      if (p.type == PowerUpType::Shield) {
        c = sf::Color(80, 200, 255);
        l = "S";
      } else if (p.type == PowerUpType::TripleShot) {
        c = sf::Color(255, 140, 0);
        l = "T";
      } else if (p.type == PowerUpType::RapidFire) {
        c = sf::Color(255, 220, 0);
        l = "R";
      } else {
        c = sf::Color(255, 0, 200);
        l = "B";
      }

      capShape.setFillColor(sf::Color(c.r, c.g, c.b, 110));
      capShape.setOutlineColor(c);
      capShape.setOutlineThickness(1.5f);
      window.draw(capShape);

      sf::Text text;
      text.setFont(font);
      text.setCharacterSize(12);
      text.setFillColor(sf::Color::White);
      text.setOutlineColor(sf::Color::Black);
      text.setOutlineThickness(1.2f);
      text.setString(l);
      sf::FloatRect tr = text.getLocalBounds();
      text.setOrigin(tr.left + tr.width / 2.f, tr.top + tr.height / 2.f);
      text.setPosition(p.pos);
      window.draw(text);
    }

    // Şok Dalgasını (Shockwave) Çiz
    if (shockwaveRadius_ < shockwaveMaxRadius_) {
      sf::CircleShape wave(shockwaveRadius_);
      wave.setOrigin(shockwaveRadius_, shockwaveRadius_);
      wave.setPosition(shockwaveCenter_);
      wave.setFillColor(sf::Color::Transparent);

      float alphaRatio = 1.f - (shockwaveRadius_ / shockwaveMaxRadius_);
      sf::Uint8 alpha = static_cast<sf::Uint8>(255.f * alphaRatio);
      wave.setOutlineColor(sf::Color(255, 0, 200, alpha));
      wave.setOutlineThickness(3.f + 5.f * alphaRatio);
      window.draw(wave);
    }

    if (!explosions.empty())
      for (auto &exp : explosions)
        exp.draw(window);
    ufo_.draw(window); // UFO en üste çizilir

    // Yüzen puan yazıları
    if (!floatingTexts_.empty())
      for (auto &ft : floatingTexts_)
        ft.draw(window);

    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);

    // Aktif Güçlendirme Süre Çubuklarını (HUD Bars) Çiz
    float barY = 40.f;
    if (shieldTimer_ > 0.f && shieldHealth_ > 0) {
      drawPowerUpBar(barY, "KALKAN", shieldTimer_, POWERUP_DURATION,
                     sf::Color(80, 200, 255));
      barY += 15.f;
    }
    if (tripleShotTimer_ > 0.f) {
      drawPowerUpBar(barY, "UCLU ATIS", tripleShotTimer_, POWERUP_DURATION,
                     sf::Color(255, 140, 0));
      barY += 15.f;
    }
    if (rapidFireTimer_ > 0.f) {
      drawPowerUpBar(barY, "HIZLI ATIS", rapidFireTimer_, POWERUP_DURATION,
                     sf::Color(255, 220, 0));
      barY += 15.f;
    }
    if (bombAmmo_ > 0) {
      bombLabelText.setFont(font);
      bombLabelText.setCharacterSize(11);
      bombLabelText.setFillColor(sf::Color(255, 0, 200)); // Parlak magenta
      bombLabelText.setString("BOMBA CEPHANESI: " + std::to_string(bombAmmo_) +
                          "  (X / LShift)");
      bombLabelText.setPosition(20.f, barY + 1.f);
      window.draw(bombLabelText);
      barY += 15.f;
    }

  } else if (gameState == State::GameOver) {
    window.draw(gameOverText);

    // Skor ve seviye bilgisi
    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(28);
    finalScoreText.setFillColor(sf::Color::White);
    finalScoreText.setString("Skor: " + std::to_string(score) +
                         "     Seviye: " + std::to_string(level));
    centreX(finalScoreText, WINDOW_WIDTH);
    finalScoreText.setPosition(finalScoreText.getPosition().x, 290.f);
    window.draw(finalScoreText);

    // High score
    // Yeni Rekor kutlaması
    if (newRecord_) {
      newRecordText.setFont(font);
      newRecordText.setCharacterSize(38);
      // Yanıp sönen sarı renk (menuTimer_ ile)
      float pulse = (std::sin(menuTimer_ * 6.f) + 1.f) / 2.f;
      sf::Uint8 alpha = static_cast<sf::Uint8>(180 + 75 * pulse);
      newRecordText.setFillColor(sf::Color(255, 255, 0, alpha));
      newRecordText.setOutlineColor(sf::Color(255, 120, 0, alpha));
      newRecordText.setOutlineThickness(2.f);
      newRecordText.setString("!!! YENI REKOR !!!");
      centreX(newRecordText, WINDOW_WIDTH);
      newRecordText.setPosition(newRecordText.getPosition().x, 400.f);
      window.draw(newRecordText);
    }

    highScoreText.setPosition(highScoreText.getPosition().x, 350.f);
    window.draw(highScoreText);

    restartHintText.setPosition(restartHintText.getPosition().x, 480.f);
    window.draw(restartHintText);

    // Seviye geçiş ekranı
  } else if (gameState == State::LevelUp) {
    drawStars(); // yıldızlar arkada görünsün

    // Büyük seviye numarası — pulse animasyonu
    lvlTitleText.setFont(font);
    lvlTitleText.setCharacterSize(72);
    float pulse = (std::sin(menuTimer_ * 4.f) + 1.f) / 2.f;
    sf::Uint8 g = static_cast<sf::Uint8>(180 + 75 * pulse);
    lvlTitleText.setFillColor(sf::Color(80, g, 255));
    lvlTitleText.setString("SEVIYE " + std::to_string(level));
    centreX(lvlTitleText, WINDOW_WIDTH);
    lvlTitleText.setPosition(lvlTitleText.getPosition().x, 200.f);
    window.draw(lvlTitleText);

    // Alt açıklama
    subTextText.setFont(font);
    subTextText.setCharacterSize(24);
    subTextText.setFillColor(sf::Color(200, 200, 200));
    subTextText.setString("Hazir Ol!");
    centreX(subTextText, WINDOW_WIDTH);
    subTextText.setPosition(subTextText.getPosition().x, 310.f);
    window.draw(subTextText);

    // Skor göster
    scoreDispText.setFont(font);
    scoreDispText.setCharacterSize(22);
    scoreDispText.setFillColor(sf::Color::Yellow);
    scoreDispText.setString("Skor: " + std::to_string(score));
    centreX(scoreDispText, WINDOW_WIDTH);
    scoreDispText.setPosition(scoreDispText.getPosition().x, 370.f);
    window.draw(scoreDispText);

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

    // Motor itki parçacıklarını çiz
    for (const auto &p : thrustParticles_) {
      window.draw(p.shape);
    }

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

  // View'ı normale döndür (sarsıntı etkisinin UI'yi sürekli kaydırmaması için)
  window.setView(window.getDefaultView());
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

// Parallax Yıldız Sistemi
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

void GameManager::addFloatingText(float x, float y, const std::string &str, sf::Color color, unsigned int characterSize, float lifeTime, float speed) {
  FloatingText ft;
  ft.text.setFont(font);
  ft.text.setCharacterSize(characterSize);
  ft.text.setFillColor(color);
  ft.text.setString(str);
  ft.x = x;
  ft.y = y;
  ft.lifeTimer = lifeTime;
  ft.maxLife = lifeTime;
  ft.speed = speed;
  ft.text.setPosition(x, y);
  floatingTexts_.push_back(ft);
}

void GameManager::drawPowerUpBar(float y, const std::string &name, float current, float maxVal, sf::Color color) {
  sf::RectangleShape bg(sf::Vector2f(100.f, 7.f));
  bg.setPosition(20.f, y + 4.f);
  bg.setFillColor(sf::Color(20, 20, 20, 160));
  bg.setOutlineColor(sf::Color(100, 100, 100, 80));
  bg.setOutlineThickness(1.f);
  window.draw(bg);
  
  float fillWidth = (current / maxVal) * 100.f;
  if (fillWidth > 100.f) fillWidth = 100.f;
  if (fillWidth < 0.f) fillWidth = 0.f;
  sf::RectangleShape fill(sf::Vector2f(fillWidth, 7.f));
  fill.setPosition(20.f, y + 4.f);
  fill.setFillColor(color);
  window.draw(fill);
  
  powerupBarLabelText.setFont(font);
  powerupBarLabelText.setCharacterSize(10);
  powerupBarLabelText.setFillColor(sf::Color(220, 220, 220));
  powerupBarLabelText.setString(name);
  powerupBarLabelText.setPosition(130.f, y + 1.f);
  window.draw(powerupBarLabelText);
}

void GameManager::spawnPowerUp(float x, float y, float dropChance) {
  float rVal = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  if (rVal > dropChance) {
    return;
  }

  PowerUp p;
  p.pos = sf::Vector2f(x, y);

  // Ağırlıklı rastgele seçim: Kalkan %35, Üçlü %30, Hızlı %25, Bomba %10
  int r = rand() % 100;
  if (r < 35) {
    p.type = PowerUpType::Shield;
  } else if (r < 65) {
    p.type = PowerUpType::TripleShot;
  } else if (r < 90) {
    p.type = PowerUpType::RapidFire;
  } else {
    p.type = PowerUpType::Bomb;
  }

  powerUps_.push_back(p);
}

void GameManager::detonateBomb(float x, float y) {
  // Patlama sesi ve sarsıntı
  sound_.playBombExplosion();
  addScreenShake(0.5f, 15.f);

  // Pembe-mor lokal şok dalgası
  shockwaveRadius_ = 0.f;
  shockwaveMaxRadius_ = 200.f; // 200 piksel blast yarıçapı
  shockwaveCenter_ = sf::Vector2f(x, y);

  // Ekrandaki tüm aktif düşman mermilerini anında sil
  enemyBullets.clear();

  // Şok dalgası içindeki tüm canlı düşmanlara hasar ver
  sf::Vector2f center(x, y);
  for (auto &enemy : enemies) {
    if (enemy.isAlive()) {
      float dx = enemy.getX() + 20.f - center.x; // düşman merkezi
      float dy = enemy.getY() + 15.f - center.y;
      float dist = std::sqrt(dx * dx + dy * dy);

      if (dist <= 200.f) {
        enemy.takeDamage(1);
        if (!enemy.isAlive()) {
          int pts = enemy.getScore();
          score += pts;

          float dropChance =
              enemy.isKamikaze() ? 0.15f : 0.05f; // Kamikaze %15, Normal %5
          spawnPowerUp(enemy.getX() + 20.f, enemy.getY() + 15.f, dropChance);

          // Yüzen bilgi yazısını tetikle
          addFloatingText(enemy.getX() + 10.f, enemy.getY() - 10.f, "+" + std::to_string(pts), sf::Color::Yellow, 14, 0.8f, 30.f);
        }
      }
    }
  }

  // Şok dalgası içindeki aktif UFO'yu vur
  if (ufo_.active) {
    float dx = ufo_.x + 23.f - center.x;
    float dy = ufo_.y + 10.f - center.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 200.f) {
      ufo_.active = false;
      sound_.stopUfo();

      static const int UFO_POINTS[] = {50, 100, 150, 300};
      int pts = UFO_POINTS[rand() % 4];
      score += pts;

      addFloatingText(ufo_.x + 10.f, ufo_.y - 10.f, "+" + std::to_string(pts), sf::Color::Cyan, 20, 1.2f, 40.f);

      explosions.emplace_back(ufo_.x + 23.f, ufo_.y + 10.f,
                              sf::Color(255, 60, 60), 18);
      spawnPowerUp(ufo_.x + 23.f, ufo_.y + 10.f, 0.50f); // %50 olasılık
    }
  }

  scoreText.setString("Skor: " + std::to_string(score));
}

sf::FloatRect GameManager::Ufo::getBounds() const {
  return sf::FloatRect(x, y, 46.f, 20.f);
}

void GameManager::Ufo::draw(sf::RenderWindow &window) {
  if (!active)
    return;

  float time = clock.getElapsedTime().asSeconds();
  float pulse = (std::sin(time * 8.f) + 1.f) / 2.f;

  // Neon Alt Işıma (Kırmızı/Turuncu Enerma)
  sf::RectangleShape glow(sf::Vector2f(34.f, 3.f));
  glow.setPosition(x + 6.f, y + 17.f);
  glow.setFillColor(
      sf::Color(255, 50, 0, static_cast<sf::Uint8>(100 + pulse * 155)));
  window.draw(glow);

  // Klasik Uçan Daire Gövdesi (Stretched Hexagon/Octagon)
  sf::ConvexShape saucer;
  saucer.setPointCount(6);
  saucer.setPoint(0, sf::Vector2f(x + 10.f, y + 5.f));  // sol üst
  saucer.setPoint(1, sf::Vector2f(x + 36.f, y + 5.f));  // sağ üst
  saucer.setPoint(2, sf::Vector2f(x + 46.f, y + 10.f)); // sağ orta
  saucer.setPoint(3, sf::Vector2f(x + 36.f, y + 15.f)); // sağ alt
  saucer.setPoint(4, sf::Vector2f(x + 10.f, y + 15.f)); // sol alt
  saucer.setPoint(5, sf::Vector2f(x, y + 10.f));        // sol orta

  saucer.setFillColor(sf::Color(180, 20, 20));
  saucer.setOutlineColor(sf::Color(255, 80, 80));
  saucer.setOutlineThickness(1.5f);
  window.draw(saucer);

  // Ortadaki Kokpit Kubbesi (Klasik Uçan Daire Camı - Pulsing Cyan)
  sf::ConvexShape dome;
  dome.setPointCount(4);
  dome.setPoint(0, sf::Vector2f(x + 16.f, y + 5.f)); // sol alt
  dome.setPoint(1, sf::Vector2f(x + 30.f, y + 5.f)); // sağ alt
  dome.setPoint(2, sf::Vector2f(x + 26.f, y + 0.f)); // sağ üst
  dome.setPoint(3, sf::Vector2f(x + 20.f, y + 0.f)); // sol üst

  dome.setFillColor(
      sf::Color(0, 220, 255, static_cast<sf::Uint8>(200 + pulse * 55)));
  dome.setOutlineColor(sf::Color(150, 255, 255));
  dome.setOutlineThickness(1.f);
  window.draw(dome);

  // Dönen Sıralı Yanıp Sönen Flaşör Işıklar (3 adet)
  sf::Color light1 = (static_cast<int>(time * 6.f) % 3 == 0)
                         ? sf::Color::Yellow
                         : sf::Color(100, 80, 0);
  sf::Color light2 = (static_cast<int>(time * 6.f) % 3 == 1)
                         ? sf::Color::Yellow
                         : sf::Color(100, 80, 0);
  sf::Color light3 = (static_cast<int>(time * 6.f) % 3 == 2)
                         ? sf::Color::Yellow
                         : sf::Color(100, 80, 0);

  // Sol ışık
  sf::RectangleShape lightL(sf::Vector2f(3.f, 3.f));
  lightL.setPosition(x + 12.f, y + 9.f);
  lightL.setFillColor(light1);
  window.draw(lightL);

  // Orta ışık
  sf::RectangleShape lightM(sf::Vector2f(3.f, 3.f));
  lightM.setPosition(x + 22.f, y + 9.f);
  lightM.setFillColor(light2);
  window.draw(lightM);

  // Sağ ışık
  sf::RectangleShape lightR(sf::Vector2f(3.f, 3.f));
  lightR.setPosition(x + 32.f, y + 9.f);
  lightR.setFillColor(light3);
  window.draw(lightR);
}