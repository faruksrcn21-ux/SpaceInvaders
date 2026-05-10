#pragma once
#include <SFML/Graphics.hpp>

enum class EnemyType { A, B, C };

class Enemy {
public:
    Enemy(float startX, float startY, EnemyType type);

    void move(float offsetX, float offsetY);
    void draw(sf::RenderWindow& window);   // artık drawA/B/C'yi çağırır
    void takeDamage(int damage);
    bool isAlive() const;
    float getX() const;
    float getY() const;
    sf::FloatRect getBounds() const;
    int getScore() const;
    EnemyType getType() const;

private:
    // tipe göre ayrı çizim fonksiyonları
    void drawA(sf::RenderWindow& window, float x, float y) const;
    void drawB(sf::RenderWindow& window, float x, float y) const;
    void drawC(sf::RenderWindow& window, float x, float y) const;

    sf::RectangleShape body;   // çarpışma kutusu (görünmez, sadece hitbox)
    int health;
    EnemyType type_;
    int animFrame;             // 0/1 — hareket animasyonu
};