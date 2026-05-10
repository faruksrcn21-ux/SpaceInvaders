#include "Bullet.h"

// Mermi görünümleri
Bullet::Bullet(float startX, float startY, float dir, sf::Color color)
    : direction(dir), speed(280.f), color_(color)  // hız ayarı: 500→280
{
    if (dir < 0) {
        // Oyuncu mermisi — lazer çekirdeği
        core.setSize(sf::Vector2f(3.f, 18.f));
        core.setFillColor(sf::Color(80, 200, 255));      // parlak mavi
        core.setOutlineColor(sf::Color(180, 240, 255));
        core.setOutlineThickness(1.5f);
        core.setOrigin(1.5f, 9.f);

        // Lazer aurası (daha geniş, yarı saydam)
        aura.setSize(sf::Vector2f(7.f, 18.f));
        aura.setFillColor(sf::Color(40, 120, 255, 80));  // yarı saydam mavi
        aura.setOrigin(3.5f, 9.f);
    } else {
        // Düşman mermisi — plazma topu
        core.setSize(sf::Vector2f(8.f, 8.f));
        core.setFillColor(sf::Color(255, 100, 0));        // turuncu
        core.setOutlineColor(sf::Color(255, 200, 0));
        core.setOutlineThickness(1.5f);
        core.setOrigin(4.f, 4.f);

        // Plazma hale
        aura.setSize(sf::Vector2f(14.f, 14.f));
        aura.setFillColor(sf::Color(255, 60, 0, 60));    // yarı saydam
        aura.setOrigin(7.f, 7.f);
    }

    core.setPosition(startX, startY);
    aura.setPosition(startX, startY);
}

void Bullet::update(float deltaTime) {
    float dy = speed * direction * deltaTime;
    core.move(0.f, dy);
    aura.move(0.f, dy);
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(aura);   // önce hale
    window.draw(core);   // üstüne çekirdek
}

float Bullet::getY() const {
    return core.getPosition().y;
}

sf::FloatRect Bullet::getBounds() const {
    return core.getGlobalBounds();
}