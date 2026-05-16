#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(float startX, float startY, float dir = -1.0f, sf::Color color = sf::Color::Yellow, float vx = 0.f, bool isBomb = false);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    float getY() const;
    sf::FloatRect getBounds() const;
    bool isBomb() const { return isBomb_; }

private:
    // çekirdek + hale (aura) çift katman
    sf::RectangleShape core;   // asıl mermi şekli + çarpışma
    sf::RectangleShape aura;   // parlama efekti (görsel)
 
    float speed;
    float direction;
    float vx_;
    sf::Color color_;
    bool isBomb_;
};
