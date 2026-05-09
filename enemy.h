#pragma once
#include <SFML/Graphics.hpp>

// Düşman tipi — sıraya göre renk ve puan belirlenir
// Tip A (üst sıra)  : Pembe  — 30 puan  (en zor hedef)
// Tip B (orta sıra) : Turuncu — 20 puan
// Tip C (alt sıra)  : Kırmızı — 10 puan (en kolay hedef)
enum class EnemyType { A, B, C };

class Enemy {
public:
    // COMMIT 4: constructor artık tip de alıyor
    Enemy(float startX, float startY, EnemyType type);

    void move(float offsetX, float offsetY);
    void draw(sf::RenderWindow& window);
    void takeDamage(int damage);
    bool isAlive() const;
    float getX() const;
    float getY() const;
    sf::FloatRect getBounds() const;

    // COMMIT 4: puan sorgulama — GameManager bu değeri kullanır
    int getScore() const;
    EnemyType getType() const;

private:
    sf::RectangleShape shape;
    int health;
    EnemyType type_;   // hangi tip olduğunu sakla
};