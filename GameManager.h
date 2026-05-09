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

    sf::RenderWindow window;
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets;
    std::vector<Enemy> enemies;
    std::vector<Barrier> barriers;

    sf::Font font;
    sf::Text scoreText, livesText, levelText, gameOverText;

    int score;
    int lives;
    int level;
    bool isGameOver;

    float swarmSpeed;
    int swarmDirection;
    float dropDistance;
    float enemyShootTimer;
    float enemyShootInterval;
};