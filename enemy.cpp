#include "Enemy.h"

Enemy::Enemy(float startX, float startY) {
    shape.setSize(sf::Vector2f(40.0f, 30.0f));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(startX, startY);
    health = 1;
}

void Enemy::move(float offsetX, float offsetY) { shape.move(offsetX, offsetY); }
void Enemy::draw(sf::RenderWindow& window) { window.draw(shape); }
void Enemy::takeDamage(int damage) { health -= damage; }
bool Enemy::isAlive() const { return health > 0; }
float Enemy::getX() const { return shape.getPosition().x; }
float Enemy::getY() const { return shape.getPosition().y; }
sf::FloatRect Enemy::getBounds() const { return shape.getGlobalBounds(); }