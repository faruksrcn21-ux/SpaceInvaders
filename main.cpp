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


int main() {
    // 800x600 boyutunda bir oyun penceresi oluşturur
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");

    Player player;
    std::vector<Bullet> bullets; // Ekrandaki tüm mermileri tutacağımız dinamik dizi
    
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


        // Ekranı temizle (Siyah renk)
        window.clear();
        
        player.draw(window); // Gemiyi çiz
        for (auto& bullet : bullets) {
            bullet.draw(window); // Aktif olan tüm mermileri çiz
        }
        
        window.display();
    }
    
    


    return 0;
}