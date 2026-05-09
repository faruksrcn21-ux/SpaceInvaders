#include "Bullet.h"

Bullet::Bullet(float startX, float startY, float dir, sf::Color color) {
    shape.setSize(sf::Vector2f(5.0f, 15.0f));
    shape.setFillColor(color);
    shape.setPosition(startX, startY);
    speed = 500.0f;
    direction = dir;
}

void Bullet::update(float deltaTime) {
    shape.move(0.0f, speed * direction * deltaTime);
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

float Bullet::getY() const { return shape.getPosition().y; }
sf::FloatRect Bullet::getBounds() const { return shape.getGlobalBounds(); }