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

    sf::RenderWindow window;
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets;
    std::vector<Enemy> enemies;
    std::vector<Barrier> barriers;
    std::vector<Explosion> explosions;
    SoundManager sound_;
    float kamikazeTimer_    = 0.f;
    float kamikazeInterval_ = 8.f;  // her 8 saniyede bir kamikaze dalar
    sf::Font font;
    sf::Text scoreText, livesText, levelText, gameOverText;
    sf::Text menuTitleText, menuSubText, restartHintText;
    sf::Text pauseText, pauseHintText, soundStatusText;
    sf::Text highScoreText;  // menüde ve GameOver'da gösterilecek
    
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