#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Patlama particle sistemi
//
// Bir düşman veya oyuncu hasar aldığında bu sınıfın bir örneği
// oluşturulur. Her patlama N adet parçacık (particle) içerir;
// parçacıklar rastgele yönlere saçılır, 0.5 saniye içinde solar
// ve sonra silinir.

struct Particle {
    sf::RectangleShape shape;
    sf::Vector2f       velocity;
    float              lifetime;    // kalan ömür (saniye)
    float              maxLifetime; // başlangıç ömrü (fade için)
};

class Explosion {
public:
    // cx, cy: patlama merkezi   color: parçacık rengi
    Explosion(float cx, float cy, sf::Color color, int count = 12);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    bool isFinished() const;  // tüm parçacıklar öldüyse true

private:
    std::vector<Particle> particles_;
};