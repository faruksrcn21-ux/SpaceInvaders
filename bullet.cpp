#include "Bullet.h"

// Mermi görünümleri
Bullet::Bullet(float startX, float startY, float dir, sf::Color color, float vx, bool isBomb)
    : direction(dir), speed(280.f), vx_(vx), color_(color), isBomb_(isBomb)
{
    if (isBomb) {
        speed = 180.f; // Yavaş, görkemli süzülen bomba mermisi
        
        core.setSize(sf::Vector2f(16.f, 16.f));
        core.setFillColor(sf::Color(255, 0, 200));      // mor-pembe (magenta) çekirdek
        core.setOutlineColor(sf::Color(255, 120, 255));
        core.setOutlineThickness(2.5f);
        core.setOrigin(8.f, 8.f);

        aura.setSize(sf::Vector2f(28.f, 28.f));
        aura.setFillColor(sf::Color(255, 0, 220, 80));   // geniş parıltılı hale
        aura.setOrigin(14.f, 14.f);
    }
    else if (dir < 0) {
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
        
        // Eğer yatay hızı varsa, mermiyi hafif eğelim!
        if (std::abs(vx) > 0.01f) {
            core.setRotation(vx > 0.f ? 15.f : -15.f);
            aura.setRotation(vx > 0.f ? 15.f : -15.f);
        }
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
    float dx = vx_ * deltaTime;
    float dy = speed * direction * deltaTime;
    core.move(dx, dy);
    aura.move(dx, dy);
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