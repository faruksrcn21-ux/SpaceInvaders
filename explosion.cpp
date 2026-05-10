#include "Explosion.h"
#include <cstdlib>
#include <cmath>

static float randF(float lo, float hi) {
    return lo + (float)rand() / RAND_MAX * (hi - lo);
}

Explosion::Explosion(float cx, float cy, sf::Color color, int count) {
    particles_.reserve(count);

    for (int i = 0; i < count; i++) {
        Particle p;

        // Boyut: küçük kare parçacıklar (3–7 px)
        float size = randF(3.f, 7.f);
        p.shape.setSize(sf::Vector2f(size, size));
        p.shape.setOrigin(size / 2.f, size / 2.f);
        p.shape.setPosition(cx, cy);
        p.shape.setFillColor(color);

        // Rastgele yön — 360 derece
        float angle = randF(0.f, 2.f * 3.14159f);
        float speed = randF(60.f, 200.f);
        p.velocity  = { std::cos(angle) * speed, std::sin(angle) * speed };

        // Ömür: 0.35 – 0.6 saniye
        p.maxLifetime = randF(0.35f, 0.6f);
        p.lifetime    = p.maxLifetime;

        particles_.push_back(p);
    }
}

void Explosion::update(float dt) {
    for (auto& p : particles_) {
        if (p.lifetime <= 0.f) continue;

        p.lifetime -= dt;
        p.shape.move(p.velocity * dt);

        // Hız yavaşlasın (sürtünme)
        p.velocity *= (1.f - 3.f * dt);

        // Alpha fade: lifetime/maxLifetime → 1.0'dan 0.0'a
        float ratio = std::max(0.f, p.lifetime / p.maxLifetime);
        sf::Color col = p.shape.getFillColor();
        col.a = static_cast<sf::Uint8>(255.f * ratio);
        p.shape.setFillColor(col);
    }
}

void Explosion::draw(sf::RenderWindow& window) const {
    for (const auto& p : particles_) {
        if (p.lifetime > 0.f)
            window.draw(p.shape);
    }
}

bool Explosion::isFinished() const {
    for (const auto& p : particles_)
        if (p.lifetime > 0.f) return false;
    return true;
}