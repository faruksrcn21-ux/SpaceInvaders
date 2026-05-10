#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"

class Player {
public:
    Player();
    void update(float deltaTime, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

private:
    // tek dikdörtgen yerine çoklu shape ile gemi
    // Ana gövde + iki kanat + kabin + iki egzoz
    sf::ConvexShape  body;       // ana üçgen gövde
    sf::RectangleShape wing_L;   // sol kanat
    sf::RectangleShape wing_R;   // sağ kanat
    sf::RectangleShape cockpit;  // kabin (üst orta)
    sf::RectangleShape exhaust_L;// sol egzoz
    sf::RectangleShape exhaust_R;// sağ egzoz
 
    float movementSpeed;
    float shootTimer;
    float shootCooldown;
 
    // Pozisyon merkezi — tüm parçaları birlikte hareket ettirmek için
    sf::Vector2f pos;
 
    void updateShapes(); // pos değişince tüm parçaları güncelle
};