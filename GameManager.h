#pragma once
#include "Barrier.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Explosion.h"
#include "Player.h"
#include "SoundManager.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class GameManager {
public:
  GameManager();
  void run();

private:
  void processEvents();
  void update(float deltaTime);
  void render();
  void initLevel();
  void resetGame();

  // Alt güncellemeler (Step 3)
  void updateEntities(float deltaTime);
  void checkCollisions();
  void checkGameState();

  // Helper fonksiyonlar
  static void centreX(sf::Text &t, float windowWidth);
  void addFloatingText(float x, float y, const std::string &str, sf::Color color, unsigned int characterSize = 14, float lifeTime = 0.8f, float speed = 30.f);
  void drawPowerUpBar(float y, const std::string &name, float current, float maxVal, sf::Color color);
  void playerHit(bool isKamikaze = false);
  bool checkBarrierCollision(const sf::FloatRect &bounds);
  void loadHighScore();
  void saveHighScore();
  void initStars();
  void updateStars(float dt);
  void drawStars();
  void spawnPowerUp(float x, float y, float dropChance);
  void detonateBomb(float x, float y);

  sf::RenderWindow window;
  Player player;
  std::vector<Bullet> bullets;
  std::vector<Bullet> enemyBullets;
  std::vector<Enemy> enemies;
  std::vector<Barrier> barriers;
  std::vector<Explosion> explosions;

  // UFO (Mothership) sistemi
  struct Ufo {
    float x = 0.f, y = 28.f;
    bool active = false;
    int direction = 1; // +1 soldan sağa, -1 sağdan sola
    float speed = 130.f;
    sf::Clock clock;   // Yerel olmayan sf::Clock üye değişkeni

    sf::FloatRect getBounds() const;
    void draw(sf::RenderWindow &window);
  };
  Ufo ufo_;
  float ufoSpawnTimer_ = 0.f;
  float ufoSpawnInterval_ = 20.f; // ilk çıkış 20 saniye sonra

  SoundManager sound_;
  float kamikazeTimer_ = 0.f;
  float kamikazeInterval_ = 8.f; // her 8 saniyede bir kamikaze dalar
  sf::Font font;
  sf::Text scoreText, livesText, levelText, gameOverText;
  sf::Text menuTitleText, menuSubText, restartHintText;
  sf::Text pauseText, pauseHintText, soundStatusText;
  sf::Text highScoreText; // menüde ve GameOver'da gösterilecek
  
  // Tekrar kullanılabilir (reusable) HUD ve Menü Yazıları
  sf::Text nameText, ptsText, controlsText, enterHintText;
  sf::Text bombLabelText, finalScoreText, newRecordText;
  sf::Text lvlTitleText, subTextText, scoreDispText;
  sf::Text powerupBarLabelText;

  // Menü animasyon zamanlayıcısı
  float menuTimer_ = 0.f;

  float levelUpTimer_ = 0.f; // 2sn gösterim sayacı
  static constexpr float LEVELUP_DURATION = 2.0f;
  static constexpr float POWERUP_DURATION = 7.0f;
  bool newRecord_ = false; // GameOver'da "Yeni Rekor!" göstermek için

  // Parallax yıldız arkaplan sistemi
  struct Star {
    float x, y, speed, size;
    sf::Uint8 brightness;
  };
  static constexpr int STAR_LAYERS = 3;
  static constexpr int STARS_PER_LAYER = 40;
  std::vector<Star> stars_;

  // Yüzen Puan Yazıları (Floating Text)
  struct FloatingText {
    sf::Text text;
    float x, y;
    float lifeTimer;
    float maxLife;
    float speed;

    void update(float dt) {
      y -= speed * dt;
      lifeTimer -= dt;
      if (lifeTimer < 0.f)
        lifeTimer = 0.f;
      sf::Color c = text.getFillColor();
      c.a = static_cast<sf::Uint8>(255.f * (lifeTimer / maxLife));
      text.setFillColor(c);
      text.setPosition(x, y);
    }
    void draw(sf::RenderWindow &w) const {
      if (lifeTimer > 0.f)
        w.draw(text);
    }
  };
  std::vector<FloatingText> floatingTexts_;

  // İtki (Motor Alevi) Parçacıkları
  struct ThrustParticle {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
  };
  std::vector<ThrustParticle> thrustParticles_;

  // Power-up (Güçlendirme) Sistemi
  enum class PowerUpType { Shield, TripleShot, RapidFire, Bomb };
  struct PowerUp {
    PowerUpType type;
    sf::Vector2f pos;
    float speed = 120.f;
    float rotation = 0.f;

    sf::FloatRect getBounds() const {
      return sf::FloatRect(pos.x - 6.f, pos.y - 9.f, 12.f, 18.f);
    }
  };
  std::vector<PowerUp> powerUps_;

  // Aktif Güçlendirme Sayaçları ve Durumları
  float shieldTimer_ = 0.f;
  float tripleShotTimer_ = 0.f;
  float rapidFireTimer_ = 0.f;
  int shieldHealth_ = 0; // Kalkanın emebileceği darbe sayısı (max 1)
  int bombAmmo_ = 0;     // Bomba mühimmatı sayısı (X ile atılır)

  // Bomba Şok Dalgası (Shockwave) Efekti
  float shockwaveRadius_ = 0.f;
  float shockwaveMaxRadius_ = 400.f;
  sf::Vector2f shockwaveCenter_;

  // Ekran Sarsıntısı (Screen Shake)
  float shakeTimer_ = 0.f;
  float shakeMagnitude_ = 0.f;
  void addScreenShake(float duration, float magnitude) {
    shakeTimer_ = duration;
    shakeMagnitude_ = magnitude;
  }
  int score;
  int lives;
  int level;
  int highScore = 0;

  enum class State { Menu, Playing, Paused, GameOver, LevelUp } gameState;

  float swarmSpeed;
  int swarmDirection;
  float dropDistance;
  float enemyShootTimer;
  float enemyShootInterval;

  bool dropPending;        // true -> bir sonraki adımda aşağı in + yön değiştir
  float swarmMoveTimer;    // adım zamanlayıcısı
  float swarmMoveInterval; // adımlar arası süre (saniye); düşman azaldıkça
                           // kısalır

  // oyun sabitleri (sihirli sayılar yerine)
  static constexpr float WINDOW_WIDTH = 800.f;
  static constexpr float WINDOW_HEIGHT = 600.f;
  static constexpr float BARRIER_Y = 450.f;
  static constexpr float RIGHT_BOUND = 760.f;
  static constexpr float LEFT_BOUND = 0.f;
  static constexpr float ENEMY_BOTTOM_LIMIT = 520.f;

  // oyuncu dokunulmazlık
  float invincibleTimer;                      // > 0 iken oyuncu hasar almaz
  static constexpr float INV_DURATION = 2.0f; // saniye
  static constexpr float BLINK_RATE = 0.12f;  // saniye — yanıp sönme hızı
  float blinkTimer;                           // 0..BLINK_RATE arası sayar
  bool playerVisible;                         // yanıp sönme için çizim flag'i
};