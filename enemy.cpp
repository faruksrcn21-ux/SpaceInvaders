#include "Enemy.h"
#include <cmath>

// Düşman görünümleri — tipe göre farklı şekil
Enemy::Enemy(float startX, float startY, EnemyType type) : type_(type) {
  // Tüm tipler için temel gövde boyutu aynı kalıyor
  // (çarpışma kutusu değişmesin)
  body.setSize(sf::Vector2f(40.0f, 30.0f));
  body.setPosition(startX, startY);

  switch (type_) {
  case EnemyType::A:
    body.setFillColor(sf::Color(255, 0, 200)); // pembe
    body.setOutlineColor(sf::Color(255, 100, 230));
    break;
  case EnemyType::B:
    body.setFillColor(sf::Color(255, 140, 0)); // turuncu
    body.setOutlineColor(sf::Color(255, 180, 60));
    break;
  case EnemyType::C:
    body.setFillColor(sf::Color(255, 40, 40)); // kırmızı
    body.setOutlineColor(sf::Color(255, 100, 100));
    break;
  }
  body.setOutlineThickness(1.5f);

  health = 1;
  animFrame = 0;
}

void Enemy::move(float offsetX, float offsetY) {
  body.move(offsetX, offsetY);
  // Animasyon karesi her harekette değişir (bacak/kol titreşimi)
  animFrame = 1 - animFrame;
}

// Tip A — UFO görünümü
// Gövde elipse benziyor, üstünde küçük kubbe
void Enemy::drawA(sf::RenderWindow &window, float x, float y) const {
  sf::Color col(255, 0, 200);
  sf::Color light(255, 120, 240);

  // Ana elips gövde (yassı dikdörtgen + kenar yuvarlama ile)
  sf::RectangleShape hull(sf::Vector2f(36.f, 14.f));
  hull.setFillColor(col);
  hull.setOutlineColor(light);
  hull.setOutlineThickness(1.5f);
  hull.setPosition(x + 2.f, y + 12.f);
  window.draw(hull);

  // Üst kubbe
  sf::RectangleShape dome(sf::Vector2f(16.f, 10.f));
  dome.setFillColor(light);
  dome.setPosition(x + 12.f, y + 4.f);
  window.draw(dome);

  // Sol anten
  sf::RectangleShape antL(sf::Vector2f(3.f, 8.f));
  antL.setFillColor(light);
  antL.setPosition(x + 6.f, y);
  window.draw(antL);

  // Sağ anten
  sf::RectangleShape antR(sf::Vector2f(3.f, 8.f));
  antR.setFillColor(light);
  antR.setPosition(x + 31.f, y);
  window.draw(antR);

  // Alt ışık yuvarlakları (animasyonlu renk)
  sf::Color glow =
      (animFrame == 0) ? sf::Color(255, 255, 0) : sf::Color(255, 180, 0);
  for (int i = 0; i < 3; i++) {
    sf::RectangleShape dot(sf::Vector2f(5.f, 5.f));
    dot.setFillColor(glow);
    dot.setPosition(x + 7.f + i * 10.f, y + 20.f);
    window.draw(dot);
  }
}

// Tip B — Yengeç görünümü
void Enemy::drawB(sf::RenderWindow &window, float x, float y) const {
  sf::Color col(255, 140, 0);
  sf::Color light(255, 200, 80);

  // Ana gövde
  sf::RectangleShape hull(sf::Vector2f(24.f, 18.f));
  hull.setFillColor(col);
  hull.setOutlineColor(light);
  hull.setOutlineThickness(1.5f);
  hull.setPosition(x + 8.f, y + 6.f);
  window.draw(hull);

  // Sol kol (animasyonlu — yukarı/aşağı)
  float armY = (animFrame == 0) ? y + 4.f : y + 8.f;
  sf::RectangleShape armL(sf::Vector2f(10.f, 6.f));
  armL.setFillColor(col);
  armL.setPosition(x, armY);
  window.draw(armL);

  // Sol kol ucu (pens)
  sf::RectangleShape clawL(sf::Vector2f(6.f, 4.f));
  clawL.setFillColor(light);
  clawL.setPosition(x - 4.f, armY - 2.f);
  window.draw(clawL);

  // Sağ kol
  sf::RectangleShape armR(sf::Vector2f(10.f, 6.f));
  armR.setFillColor(col);
  armR.setPosition(x + 30.f, armY);
  window.draw(armR);

  // Sağ kol ucu
  sf::RectangleShape clawR(sf::Vector2f(6.f, 4.f));
  clawR.setFillColor(light);
  clawR.setPosition(x + 38.f, armY - 2.f);
  window.draw(clawR);

  // Alt bacaklar
  for (int i = 0; i < 3; i++) {
    sf::RectangleShape leg(sf::Vector2f(4.f, 7.f));
    leg.setFillColor(col);
    leg.setPosition(x + 10.f + i * 8.f, y + 22.f);
    window.draw(leg);
  }

  // Göz çiftleri
  sf::RectangleShape eyeL(sf::Vector2f(5.f, 5.f));
  eyeL.setFillColor(sf::Color::White);
  eyeL.setPosition(x + 11.f, y + 9.f);
  window.draw(eyeL);

  sf::RectangleShape eyeR(sf::Vector2f(5.f, 5.f));
  eyeR.setFillColor(sf::Color::White);
  eyeR.setPosition(x + 24.f, y + 9.f);
  window.draw(eyeR);
}

// Tip C — Böcek görünümü
void Enemy::drawC(sf::RenderWindow &window, float x, float y) const {
  sf::Color col(255, 40, 40);
  sf::Color light(255, 120, 120);

  // Ana gövde
  sf::RectangleShape hull(sf::Vector2f(28.f, 16.f));
  hull.setFillColor(col);
  hull.setOutlineColor(light);
  hull.setOutlineThickness(1.5f);
  hull.setPosition(x + 6.f, y + 7.f);
  window.draw(hull);

  // Üst çıkıntılar (kafa boynuzu)
  sf::RectangleShape hornL(sf::Vector2f(4.f, 7.f));
  hornL.setFillColor(light);
  hornL.setPosition(x + 10.f, y);
  window.draw(hornL);

  sf::RectangleShape hornR(sf::Vector2f(4.f, 7.f));
  hornR.setFillColor(light);
  hornR.setPosition(x + 26.f, y);
  window.draw(hornR);

  // Sol bacaklar (animasyonlu)
  float legOffset = (animFrame == 0) ? 0.f : 3.f;
  sf::RectangleShape legL1(sf::Vector2f(8.f, 3.f));
  legL1.setFillColor(col);
  legL1.setPosition(x, y + 10.f + legOffset);
  window.draw(legL1);

  sf::RectangleShape legL2(sf::Vector2f(8.f, 3.f));
  legL2.setFillColor(col);
  legL2.setPosition(x, y + 18.f - legOffset);
  window.draw(legL2);

  // Sağ bacaklar
  sf::RectangleShape legR1(sf::Vector2f(8.f, 3.f));
  legR1.setFillColor(col);
  legR1.setPosition(x + 32.f, y + 10.f + legOffset);
  window.draw(legR1);

  sf::RectangleShape legR2(sf::Vector2f(8.f, 3.f));
  legR2.setFillColor(col);
  legR2.setPosition(x + 32.f, y + 18.f - legOffset);
  window.draw(legR2);

  // Göz çiftleri
  sf::RectangleShape eyeL(sf::Vector2f(5.f, 5.f));
  eyeL.setFillColor(sf::Color::Yellow);
  eyeL.setPosition(x + 10.f, y + 10.f);
  window.draw(eyeL);

  sf::RectangleShape eyeR(sf::Vector2f(5.f, 5.f));
  eyeR.setFillColor(sf::Color::Yellow);
  eyeR.setPosition(x + 25.f, y + 10.f);
  window.draw(eyeR);
}

void Enemy::draw(sf::RenderWindow &window) {
  float x = body.getPosition().x;
  float y = body.getPosition().y;

  switch (type_) {
  case EnemyType::A:
    drawA(window, x, y);
    break;
  case EnemyType::B:
    drawB(window, x, y);
    break;
  case EnemyType::C:
    drawC(window, x, y);
    break;
  }
}

void Enemy::takeDamage(int damage) { health -= damage; }
bool Enemy::isAlive() const { return health > 0; }
float Enemy::getX() const { return body.getPosition().x; }
float Enemy::getY() const { return body.getPosition().y; }
sf::FloatRect Enemy::getBounds() const { return body.getGlobalBounds(); }
EnemyType Enemy::getType() const { return type_; }

int Enemy::getScore() const {
  switch (type_) {
  case EnemyType::A:
    return 30;
  case EnemyType::B:
    return 20;
  case EnemyType::C:
    return 10;
  }
  return 10;
}

// Kamikaze implementasyonu
// activateKamikaze: hedef pozisyona doğru hız vektörü hesapla
void Enemy::activateKamikaze(float targetX, float targetY) {
  if (type_ != EnemyType::C)
    return; // sadece Tip C
  kamikaze_ = true;

  // Düşmandan hedefe yön vektörü
  float cx = body.getPosition().x + 20.f; // düşman merkezi
  float cy = body.getPosition().y + 15.f;
  float dx = targetX - cx;
  float dy = targetY - cy;
  float len = std::sqrt(dx * dx + dy * dy);
  if (len < 1.f)
    len = 1.f;

  // Normalleştir + hız uygula
  kamikazeVel_ = {(dx / len) * kamikazeSpeed_, (dy / len) * kamikazeSpeed_};
}

// updateKamikaze: her frame bağımsız hareket
void Enemy::updateKamikaze(float dt) {
  if (!kamikaze_ || !isAlive())
    return;
  body.move(kamikazeVel_ * dt);
  animFrame = 1 - animFrame; // hızlı animasyon
}

// hasReachedTarget: ekran dışına çıktıysa temizle
bool Enemy::hasReachedTarget() const {
  float y = body.getPosition().y;
  float x = body.getPosition().x;
  return (y > 620.f || y < -50.f || x < -60.f || x > 860.f);
}