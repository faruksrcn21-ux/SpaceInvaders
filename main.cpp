#include <SFML/Graphics.hpp>
#include <vector>

// --- MERMİ (BULLET) SINIFI ---
class Bullet {
public:
    Bullet(float startX, float startY) {
        // Merminin boyutu ve rengi (Küçük sarı bir lazer)
        shape.setSize(sf::Vector2f(5.0f, 15.0f));
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(startX, startY);
        speed = 500.0f; // Merminin yukarı gidiş hızı
    }

    void update(float deltaTime) {
        // Y ekseninde eksi (-) yönde gitmek yukarı çıkmaktır
        shape.move(0.0f, -speed * deltaTime); 
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    float getY() const { 
        return shape.getPosition().y; 
    }

    sf::FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }

private:
    sf::RectangleShape shape;
    float speed;
};

// --- OYUNCU (PLAYER) SINIFI ---
class Player {
public:
    Player() {
        // Gemi şimdilik yeşil bir dikdörtgen
        shape.setSize(sf::Vector2f(50.0f, 30.0f));
        shape.setFillColor(sf::Color::Green);
        // Ekranın alt ortasına yerleştir
        shape.setPosition(375.0f, 550.0f); 
        
        movementSpeed = 300.0f;
        shootTimer = 0.0f;
        shootCooldown = 0.2f; // Boşluğa basılı tutunca saniyede 5 mermi atar
    }


    void update(float deltaTime, std::vector<Bullet>& bullets) {
        // 1. Hareket Kontrolleri
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            shape.move(-movementSpeed * deltaTime, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            shape.move(movementSpeed * deltaTime, 0.0f);
        }

        // 2. Ekran Sınırları
        if (shape.getPosition().x < 0) shape.setPosition(0, shape.getPosition().y);
        if (shape.getPosition().x > 800 - 50) shape.setPosition(800 - 50, shape.getPosition().y);

        // 3. Ateş Etme Kontrolü
        shootTimer += deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && shootTimer >= shootCooldown) {
            // Mermiyi geminin tam ortasından (X ekseni) ve ucundan (Y ekseni) çıkart
            float bulletX = shape.getPosition().x + (shape.getSize().x / 2.0f) - 2.5f;
            float bulletY = shape.getPosition().y;
            
            bullets.push_back(Bullet(bulletX, bulletY)); // Yeni mermiyi listeye ekle
            shootTimer = 0.0f; // Ateş ettikten sonra sayacı sıfırla
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

private:
    sf::RectangleShape shape;
    float movementSpeed;
    float shootTimer;
    float shootCooldown;
};

// --- DÜŞMAN SINIFI ---
class Enemy {
public:
    Enemy(float startX, float startY) {
        // Düşmanlarımız şimdilik kırmızı kutular
        shape.setSize(sf::Vector2f(40.0f, 30.0f));
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(startX, startY);
        health = 1; // Düşmanın başlangıç sağlığı
    }

    void move(float offsetX, float offsetY) {
        shape.move(offsetX, offsetY);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    void takeDamage(int damage) {
        health -= damage;
    }

    bool isAlive() const {
        return health > 0;
    }

    sf::FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }

    float getX() const { return shape.getPosition().x; }
    float getY() const { return shape.getPosition().y; }

private:
    sf::RectangleShape shape;
    int health;
};
int main() {
    // 800x600 boyutunda bir oyun penceresi oluşturur
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");

    Player player;
    std::vector<Bullet> bullets; // Ekrandaki tüm mermileri tutacağımız dinamik dizi
    // Düşman Sürüsünü Oluşturma (Grid Mantığı)
    std::vector<Enemy> enemies;
    int rows = 4;
    int cols = 8;
    float startX = 50.0f;
    float startY = 50.0f;
    float spacingX = 60.0f; // Yan yana olanlar arasındaki boşluk
    float spacingY = 50.0f; // Alt alta olanlar arasındaki boşluk

    // İki boyutlu matris gibi düşmanları yerleştir
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            enemies.push_back(Enemy(startX + j * spacingX, startY + i * spacingY));
        }
    }

    // Sürü Hareket Değişkenleri
    float swarmSpeed = 100.0f; // Sürünün genel hızı
    int swarmDirection = 1;    // 1 sağa, -1 sola demek
    float dropDistance = 20.0f;// Duvara çarpınca aşağı inme miktarı
    sf::Clock clock; // Delta time hesaplamak için saat
    
    // Oyun döngüsü: Pencere açık olduğu sürece çalışır
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds(); // Oyunun her bilgisayarda aynı hızda çalışmasını sağlar

        sf::Event event;
        // Kapatma tuşuna basılıp basılmadığını kontrol eder
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Oyuncuyu güncelle (hareket ve ateş etme)
        player.update(deltaTime, bullets);

        // Mermileri hareket ettirir ve ekrandan çıkanları siler
        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].update(deltaTime);
            
            // Eğer mermi ekranın en üst sınırını (0) geçerse
            if (bullets[i].getY() < 0) {
                bullets.erase(bullets.begin() + i);
                i--; // Eleman silindiği için dizinin kaymasını engellemek adına indeksi bir geri alıyoruz
            }
        }

        // Çarpışma tespiti: Mermiler ve düşmanlar
        for (int i = 0; i < bullets.size(); i++) {
            for (int j = 0; j < enemies.size(); j++) {
                if (bullets[i].getBounds().intersects(enemies[j].getBounds())) {
                    // Çarpışma oldu, düşmana hasar ver
                    enemies[j].takeDamage(1);
                    // Mermiyi sil
                    bullets.erase(bullets.begin() + i);
                    i--;
                    break; // Bu mermi için başka düşman kontrol etme
                }
            }
        }

        // Ölü düşmanları sil
        for (int i = 0; i < enemies.size(); i++) {
            if (!enemies[i].isAlive()) {
                enemies.erase(enemies.begin() + i);
                i--;
            }
        }

        // 3. Düşman Sürüsü Güncellemesi (Kovan Zihni)
        bool hitWall = false;
        for (auto& enemy : enemies) {
            enemy.move(swarmSpeed * swarmDirection * deltaTime, 0); // Hepsini kaydır
            
            // Eğer herhangi bir düşman ekranın sağ veya sol sınırına değerse
            if (enemy.getX() <= 0 || enemy.getX() >= 800 - 40.0f) {
                hitWall = true;
            }
        }

        // Eğer sürü duvara çarptıysa yön değiştir ve bir alt satıra in
        if (hitWall) {
            swarmDirection *= -1; // Yönü tersine çevir
            for (auto& enemy : enemies) {
                // Duvarın içine sıkışmamaları için onları aşağı kaydırıyoruz
                enemy.move(swarmSpeed * swarmDirection * deltaTime, dropDistance); 
            }
        }

        // Ekranı temizle (Siyah renk)
        window.clear();
        
        player.draw(window); // Gemiyi çiz
        for (auto& bullet : bullets) {
            bullet.draw(window); // Aktif olan tüm mermileri çiz
            
        }
        for (auto& enemy : enemies) {
            enemy.draw(window);
        }
        window.display();
    }
    
    


    return 0;
}