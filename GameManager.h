#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"
#include "Enemy.h"
#include "Barrier.h"
#include "Bullet.h"

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

    sf::RenderWindow window;
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets;
    std::vector<Enemy> enemies;
    std::vector<Barrier> barriers;

    sf::Font font;
    sf::Text scoreText, livesText, levelText, gameOverText;
    sf::Text menuTitleText, menuSubText, winText, winSubText, restartHintText;

    bool fontLoaded;

    int score;
    int lives;
    int level;
    bool isGameOver;

    enum class State { Menu, Playing, GameOver, Win } gameState;

    float swarmSpeed;
    int   swarmDirection;
    float dropDistance;
    float enemyShootTimer;
    float enemyShootInterval;


    bool  dropPending;      // true -> bir sonraki adımda aşağı in + yön değiştir
    float swarmMoveTimer;   // adım zamanlayıcısı
    float swarmMoveInterval;// adımlar arası süre (saniye); düşman azaldıkça kısalır

    // oyuncu dokunulmazlık
    float invincibleTimer;          // > 0 iken oyuncu hasar almaz
    static constexpr float INV_DURATION = 2.0f;   // saniye
    static constexpr float BLINK_RATE   = 0.12f;  // saniye — yanıp sönme hızı
    float blinkTimer;               // 0..BLINK_RATE arası sayar
    bool  playerVisible;            // yanıp sönme için çizim flag'i
};