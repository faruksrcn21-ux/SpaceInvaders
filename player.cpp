#include "Player.h"

Player::Player() {
    shape.setSize(sf::Vector2f(50.0f, 30.0f));
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(375.0f, 550.0f);
    movementSpeed = 300.0f;
    shootTimer = 0.0f;
    shootCooldown = 0.2f;
}

void Player::update(float deltaTime, std::vector<Bullet>& bullets) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) shape.move(-movementSpeed * deltaTime, 0.0f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) shape.move(movementSpeed * deltaTime, 0.0f);

    if (shape.getPosition().x < 0) shape.setPosition(0, shape.getPosition().y);
    if (shape.getPosition().x > 800 - 50) shape.setPosition(800 - 50, shape.getPosition().y);

    shootTimer += deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && shootTimer >= shootCooldown) {
        float bulletX = shape.getPosition().x + (shape.getSize().x / 2.0f) - 2.5f;
        float bulletY = shape.getPosition().y;
        bullets.push_back(Bullet(bulletX, bulletY));
        shootTimer = 0.0f;
    }
}

void Player::draw(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Player::getBounds() const { return shape.getGlobalBounds(); }