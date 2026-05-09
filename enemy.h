#pragma once
#include <SFML/Graphics.hpp>

class Enemy {
public:
    Enemy(float startX, float startY);
    void move(float offsetX, float offsetY);
    void draw(sf::RenderWindow& window);
    void takeDamage(int damage);
    bool isAlive() const;
    float getX() const;
    float getY() const;
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    int health;
};