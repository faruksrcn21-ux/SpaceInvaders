#include "Player.h"

// Oyuncu gemisi görünümü
Player::Player() {
  movementSpeed = 300.0f;
  shootTimer = 0.0f;
  shootCooldown = 0.45f; // hız ayarı: 0.2→0.45 (daha az sık ateş)
  pos = sf::Vector2f(375.0f, 550.0f);

  // Ana gövde — sivri üçgen
  body.setPointCount(3);
  body.setFillColor(sf::Color(0, 220, 80));
  body.setOutlineColor(sf::Color(0, 255, 100));
  body.setOutlineThickness(1.5f);

  // Sol kanat
  wing_L.setSize(sf::Vector2f(12.f, 8.f));
  wing_L.setFillColor(sf::Color(0, 190, 70));
  wing_L.setOutlineColor(sf::Color(0, 230, 90));
  wing_L.setOutlineThickness(1.f);

  // Sağ kanat
  wing_R.setSize(sf::Vector2f(12.f, 8.f));
  wing_R.setFillColor(sf::Color(0, 190, 70));
  wing_R.setOutlineColor(sf::Color(0, 230, 90));
  wing_R.setOutlineThickness(1.f);

  // Kabin
  cockpit.setSize(sf::Vector2f(12.f, 8.f));
  cockpit.setFillColor(sf::Color(80, 200, 255));
  cockpit.setOutlineColor(sf::Color(140, 230, 255));
  cockpit.setOutlineThickness(1.f);

  // Sol egzoz
  exhaust_L.setSize(sf::Vector2f(5.f, 8.f));
  exhaust_L.setFillColor(sf::Color(255, 140, 0));

  // Sağ egzoz
  exhaust_R.setSize(sf::Vector2f(5.f, 8.f));
  exhaust_R.setFillColor(sf::Color(255, 140, 0));

  updateShapes();
}

void Player::updateShapes() {
  // pos = geminin alt-orta noktası
  float x = pos.x;
  float y = pos.y;

  // Ana gövde: 40 geniş, 30 yüksek üçgen (alt ortadan tepeye)
  body.setPoint(0, sf::Vector2f(x, y - 30.f)); // tepe
  body.setPoint(1, sf::Vector2f(x - 20.f, y)); // sol alt
  body.setPoint(2, sf::Vector2f(x + 20.f, y)); // sağ alt

  // Sol kanat: gövdenin soluna yapışık, orta yükseklikte
  wing_L.setPosition(x - 26.f, y - 12.f);

  // Sağ kanat
  wing_R.setPosition(x + 14.f, y - 12.f);

  // Kabin: tepenin hemen altında ortada
  cockpit.setPosition(x - 6.f, y - 26.f);

  // Sol egzoz: sol alt köşenin biraz içinde, alttan sarkar
  exhaust_L.setPosition(x - 14.f, y);

  // Sağ egzoz
  exhaust_R.setPosition(x + 9.f, y);
}

void Player::update(float deltaTime, std::vector<Bullet> &bullets, bool rapidFire, bool tripleShot, int &bombAmmo) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    pos.x -= movementSpeed * deltaTime;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    pos.x += movementSpeed * deltaTime;

  // Ekran sınırı (geminin toplam genişliği ~52 px)
  if (pos.x < 26.f)
    pos.x = 26.f;
  if (pos.x > 800.f - 26.f)
    pos.x = 800.f - 26.f;

  updateShapes();

  shootTimer += deltaTime;
  
  float currentCooldown = rapidFire ? (shootCooldown * 0.44f) : shootCooldown; // Hızlı ateş: %56 daha hızlı
  
  // Bomba Ateşleme: X veya Sol Shift tuşu
  bool fireBombPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::X) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
  if (fireBombPressed && bombAmmo > 0 && shootTimer >= 0.35f) {
    bullets.push_back(Bullet(pos.x, pos.y - 30.f, -1.f, sf::Color(255, 0, 200), 0.f, true)); // isBomb = true
    bombAmmo--;
    shootTimer = 0.0f;
  }
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) &&
      shootTimer >= currentCooldown) {
    if (tripleShot) {
      // Orta düz mermi
      bullets.push_back(Bullet(pos.x - 1.5f, pos.y - 30.f, -1.f, sf::Color(255, 140, 0))); // Turuncu/altın
      // Sol açılı mermi (yatay hızı -70.f)
      bullets.push_back(Bullet(pos.x - 8.f, pos.y - 20.f, -1.f, sf::Color(255, 140, 0), -70.f));
      // Sağ açılı mermi (yatay hızı +70.f)
      bullets.push_back(Bullet(pos.x + 5.f, pos.y - 20.f, -1.f, sf::Color(255, 140, 0), 70.f));
    } else {
      float bulletX = pos.x - 2.5f;
      float bulletY = pos.y - 30.f;
      bullets.push_back(Bullet(bulletX, bulletY, -1.f, sf::Color::Yellow));
    }
    shootTimer = 0.0f;
  }
}

void Player::draw(sf::RenderWindow &window) {
  // Egzozlar arkada
  window.draw(exhaust_L);
  window.draw(exhaust_R);
  // Kanatlar gövdenin arkasında
  window.draw(wing_L);
  window.draw(wing_R);
  // Ana gövde
  window.draw(body);
  // Kabin en üstte
  window.draw(cockpit);
}

sf::FloatRect Player::getBounds() const {
  // Çarpışma için ana gövdenin sınır kutusu yeterli
  return body.getGlobalBounds();
}