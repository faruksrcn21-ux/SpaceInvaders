#pragma once
#include <SFML/Graphics.hpp>

enum class EnemyType { A, B, C };

class Enemy {
public:
    Enemy(float startX, float startY, EnemyType type);

    void move(float offsetX, float offsetY);
    void draw(sf::RenderWindow& window);
    void takeDamage(int damage);
    bool isAlive() const;
    float getX() const;
    float getY() const;
    sf::FloatRect getBounds() const;
    int getScore() const;
    EnemyType getType() const;

    // Kamikaze sistemi
    // Sadece Tip C düşmanlar kamikaze moduna girebilir.
    // GameManager belirli aralıklarla activateKamikaze() çağırır;
    // bu noktadan itibaren düşman formasyon hareketini bırakır ve
    // update() içinde oyuncunun pozisyonuna doğru dalar.
    // Oyuncuya çarpınca zarar verir ve kendisi de ölür.
    void activateKamikaze(float targetX, float targetY);
    void updateKamikaze(float dt);  // bağımsız hareket — GameManager çağırır
    bool isKamikaze()  const { return kamikaze_; }
    bool hasReachedTarget() const;  // hedefe ulaştı mı (ekran dışı veya çarpışma)
    
private:
    // tipe göre ayrı çizim fonksiyonları
    void drawA(sf::RenderWindow& window, float x, float y) const;
    void drawB(sf::RenderWindow& window, float x, float y) const;
    void drawC(sf::RenderWindow& window, float x, float y) const;

    sf::RectangleShape body;   // çarpışma kutusu (görünmez, sadece hitbox)
    int health;
    EnemyType type_;
    int animFrame;             // 0/1 — hareket animasyonu

    // kamikaze durumu
    bool         kamikaze_  = false;
    sf::Vector2f kamikazeVel_;      // hız vektörü (normalleştirilmiş * speed)
    float        kamikazeSpeed_     = 220.f;  // px/sn
};