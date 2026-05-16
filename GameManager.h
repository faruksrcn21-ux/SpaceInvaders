#pragma once
#include "Barrier.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Explosion.h"
#include "Player.h"
#include "SoundManager.h"
#include <SFML/Graphics.hpp>
#include <fstream>
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

    sf::FloatRect getBounds() const { return sf::FloatRect(x, y, 46.f, 20.f); }
    void draw(sf::RenderWindow &window) const {
      if (!active)
        return;

      static sf::Clock clock;
      float time = clock.getElapsedTime().asSeconds();
      float pulse = (std::sin(time * 8.f) + 1.f) / 2.f;

      // Neon Alt Işıma (Kırmızı/Turuncu Enerji Işıması)
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

  // Menü animasyon zamanlayıcısı
  float menuTimer_ = 0.f;

  float levelUpTimer_ = 0.f; // 2sn gösterim sayacı
  static constexpr float LEVELUP_DURATION = 2.0f;
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