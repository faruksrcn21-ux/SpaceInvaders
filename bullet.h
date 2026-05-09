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
    sf::RectangleShape shape;
    float speed;
    float direction;
};
