#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(float startX, float startY, float dir = -1.0f, sf::Color color = sf::Color::Yellow);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    float getY() const;
    sf::FloatRect getBounds() const;

private:
    // çekirdek + hale (aura) çift katman
    sf::RectangleShape core;   // asıl mermi şekli + çarpışma
    sf::RectangleShape aura;   // parlama efekti (görsel)
 
    float speed;
    float direction;
    sf::Color color_;
};
