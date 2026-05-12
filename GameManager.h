#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "Player.h"
#include "Enemy.h"
#include "Barrier.h"
#include "Bullet.h"
#include "Explosion.h"
#include "SoundManager.h"

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
    bool checkBarrierCollision(const sf::FloatRect& bounds);
    void loadHighScore();
    void saveHighScore();
    void initStars();
    void updateStars(float dt);
    void drawStars();

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
        bool  active = false;
        int   direction = 1;   // +1 soldan sağa, -1 sağdan sola
        float speed = 130.f;

        sf::FloatRect getBounds() const {
            return sf::FloatRect(x, y, 46.f, 20.f);
        }
        void draw(sf::RenderWindow& window) const {
            if (!active) return;
            // Gövde
            sf::RectangleShape body(sf::Vector2f(46.f, 14.f));
            body.setPosition(x, y + 6.f);
            body.setFillColor(sf::Color(200, 30, 30));
            window.draw(body);
            // Kubbe
            sf::RectangleShape dome(sf::Vector2f(22.f, 8.f));
            dome.setPosition(x + 12.f, y);
            dome.setFillColor(sf::Color(255, 80, 80));
            window.draw(dome);
        }
    };
    Ufo   ufo_;
    float ufoSpawnTimer_    = 0.f;
    float ufoSpawnInterval_ = 20.f;  // ilk çıkış 20 saniye sonra

    SoundManager sound_;
    float kamikazeTimer_    = 0.f;
    float kamikazeInterval_ = 8.f;  // her 8 saniyede bir kamikaze dalar
    sf::Font font;
    sf::Text scoreText, livesText, levelText, gameOverText;
    sf::Text menuTitleText, menuSubText, restartHintText;
    sf::Text pauseText, pauseHintText, soundStatusText;
    sf::Text highScoreText;  // menüde ve GameOver'da gösterilecek

    // Menü animasyon zamanlayıcısı
    float menuTimer_ = 0.f;

    // Parallax yıldız arkaplan sistemi
    struct Star {
        float x, y, speed, size;
        sf::Uint8 brightness;
    };
    static constexpr int STAR_LAYERS     = 3;
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
            if (lifeTimer < 0.f) lifeTimer = 0.f;
            sf::Color c = text.getFillColor();
            c.a = static_cast<sf::Uint8>(255.f * (lifeTimer / maxLife));
            text.setFillColor(c);
            text.setPosition(x, y);
        }
        void draw(sf::RenderWindow& w) const {
            if (lifeTimer > 0.f) w.draw(text);
        }
    };
    std::vector<FloatingText> floatingTexts_;

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

    enum class State { Menu, Playing, Paused, GameOver } gameState;

    float swarmSpeed;
    int   swarmDirection;
    float dropDistance;
    float enemyShootTimer;
    float enemyShootInterval;


    bool  dropPending;      // true -> bir sonraki adımda aşağı in + yön değiştir
    float swarmMoveTimer;   // adım zamanlayıcısı
    float swarmMoveInterval;// adımlar arası süre (saniye); düşman azaldıkça kısalır

    // oyun sabitleri (sihirli sayılar yerine)
    static constexpr float WINDOW_WIDTH       = 800.f;
    static constexpr float WINDOW_HEIGHT      = 600.f;
    static constexpr float BARRIER_Y          = 450.f;
    static constexpr float RIGHT_BOUND        = 760.f;
    static constexpr float LEFT_BOUND         = 0.f;
    static constexpr float ENEMY_BOTTOM_LIMIT = 420.f;

    // oyuncu dokunulmazlık
    float invincibleTimer;          // > 0 iken oyuncu hasar almaz
    static constexpr float INV_DURATION = 2.0f;   // saniye
    static constexpr float BLINK_RATE   = 0.12f;  // saniye — yanıp sönme hızı
    float blinkTimer;               // 0..BLINK_RATE arası sayar
    bool  playerVisible;            // yanıp sönme için çizim flag'i
};