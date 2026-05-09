#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"

class Player {
public:
    Player();
    void update(float deltaTime, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    float movementSpeed;
    float shootTimer;
    float shootCooldown;
};