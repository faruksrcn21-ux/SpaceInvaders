#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// --- MERMİ SINIFI ---
class Bullet {
public:
    // dir: -1 yukarı (oyuncu), 1 aşağı (düşman)
    Bullet(float startX, float startY, float dir = -1.0f, sf::Color color = sf::Color::Yellow) {
        shape.setSize(sf::Vector2f(5.0f, 15.0f));
        shape.setFillColor(color);
        shape.setPosition(startX, startY);
        speed = 500.0f;
        direction = dir; // Merminin yönü
    }

    void update(float deltaTime) { 
        shape.move(0.0f, speed * direction * deltaTime); 
    }
    
    void draw(sf::RenderWindow& window) { window.draw(shape); }
    float getY() const { return shape.getPosition().y; }
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

private:
    sf::RectangleShape shape;
    float speed;
    float direction;
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
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

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
// --- BARİYER (BUNKER) SINIFI ---
class Barrier {
public:
    Barrier(float startX, float startY) {
        // 4 satır, 6 sütundan oluşan parçalanabilir küçük tuğlalar
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 6; j++) {
                sf::RectangleShape block(sf::Vector2f(15.0f, 15.0f));
                block.setFillColor(sf::Color::Cyan); // Açık mavi siperler
                block.setPosition(startX + j * 15.0f, startY + i * 15.0f);
                blocks.push_back(block);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& block : blocks) window.draw(block);
    }

    // Çarpışma hesaplaması için blok listesine erişim
    std::vector<sf::RectangleShape>& getBlocks() { return blocks; }

private:
    std::vector<sf::RectangleShape> blocks; // Bariyeri oluşturan küçük parçalar
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
    std::vector<Bullet> enemyBullets; // Düşman mermilerini tutacağımız liste
    float enemyShootTimer = 0.0f;
    float enemyShootInterval = 1.0f; // Saniyede ortalama 1 kere ateş edecekler
    srand((unsigned int)time(0)); // Rastgele sayı üretecini başlat
    // Ekrana 3 adet siper yerleştir
    std::vector<Barrier> barriers;
    barriers.push_back(Barrier(100.0f, 450.0f));
    barriers.push_back(Barrier(350.0f, 450.0f));
    barriers.push_back(Barrier(600.0f, 450.0f));
    bool isGameOver = false; // Oyunun bitip bitmediğini kontrol eden ana şalter
    // --- UI (ARAYÜZ) DEĞİŞKENLERİ VE FONT ---
    int score = 0;
    int lives = 3;

    sf::Font font;
    bool hasGameOverFont = true;
    // Windows'un varsayılan fontlarından birini yüklüyoruz
    if (!font.loadFromFile("C:/Windows/Fonts/consola.ttf")) {
        printf("Font yuklenemedi!\n");
        hasGameOverFont = false;
    }

    // Skor Yazısı
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, 20.f);
    scoreText.setString("Skor: 0");

    // Can Yazısı
    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(650.f, 20.f);
    livesText.setString("Can: 3");
    // Game Over Yazısı (Başlangıçta gizli olacak)
    sf::Text gameOverText;
    if (hasGameOverFont) {
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(80);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER");
        // Yazıyı ekranın tam ortasına hizalamak için matematiksel ortalama alıyoruz
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        gameOverText.setPosition(800.0f / 2.0f, 600.0f / 2.0f);
    }
    int level = 1;
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(350.f, 20.f); // Ekranın üst orta kısmına
    levelText.setString("Seviye: 1");
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

        // --- SADECE OYUN DEVAM EDİYORSA GÜNCELLE ---
        if (!isGameOver) {
            // Oyuncuyu güncelle (hareket ve ateş etme)
            player.update(deltaTime, bullets);

        // Mermileri hareket ettirir ve ekrandan çıkanları siler
        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].update(deltaTime);
            
            // Eğer mermi ekranın en üst sınırını (0) geçerse
            if (bullets[i].getY() < 0) {
                bullets.erase(bullets.begin() + i);
                // Düşman vuruldu, skoru artır ve metni güncelle
                    score += 10;
                    scoreText.setString("Skor: " + std::to_string(score));
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

        // Çarpışma tespiti: Mermiler ve barrierler
        for (int i = 0; i < bullets.size(); i++) {
            bool bulletDestroyed = false;
            for (auto& barrier : barriers) {
                auto& blocks = barrier.getBlocks();
                for (int k = 0; k < blocks.size(); k++) {
                    if (bullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                        blocks.erase(blocks.begin() + k); // Çarpan tuğlayı yok et
                        bulletDestroyed = true;
                        break;
                    }
                }
                if (bulletDestroyed) break;
            }
            if (bulletDestroyed) {
                bullets.erase(bullets.begin() + i);
                i--;
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

        // 4.Eğer sürü duvara çarptıysa yön değiştir ve bir alt satıra in
        if (hitWall) {
            swarmDirection *= -1; // Yönü tersine çevir
            for (auto& enemy : enemies) {
                // Duvarın içine sıkışmamaları için onları aşağı kaydırıyoruz
                enemy.move(swarmSpeed * swarmDirection * deltaTime, dropDistance); 
            }
        }
        // 5. Düşman Ateş Etme Mantığı
        enemyShootTimer += deltaTime;
        if (enemyShootTimer >= enemyShootInterval && !enemies.empty()) {
            // Rastgele bir düşman seç
            int randomIndex = rand() % enemies.size();
            float startX = enemies[randomIndex].getX() + 20.0f; // Düşmanın tam ortası
            float startY = enemies[randomIndex].getY() + 30.0f; // Düşmanın alt ucu
            
            enemyBullets.push_back(Bullet(startX, startY, 1.0f, sf::Color::Red));
            
            // Ateş süresini rastgeleleştirir (0.5 ile 1.5 saniye arası) ki tahmin edilemez olsun
            enemyShootTimer = 0.0f;
            enemyShootInterval = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
        }

        // 6. Düşman Mermilerini Güncelleme ve Oyuncu Çarpışması
        for (int i = 0; i < enemyBullets.size(); i++) {
            enemyBullets[i].update(deltaTime); // Mermiyi aşağı kaydır
            
            // Mermi oyuncuya çarptı mı?
            bool eBulletDestroyed = false;

            // Düşman mermisi gemimize çarptı mı?
            if (enemyBullets[i].getBounds().intersects(player.getBounds())) {
                lives--; // Canı 1 azalt
                livesText.setString("Can: " + std::to_string(lives)); // Ekrana yansıt
                eBulletDestroyed = true;

                if (lives <= 0) {
                    isGameOver = true; // Şalteri indir!
                    window.setTitle("Space Invaders - GAME OVER!"); // Pencere başlığına yaz
                }
            }

            // Düşman mermisi bariyere çarptı mı?
            if (!eBulletDestroyed) {
                for (auto& barrier : barriers) {
                    auto& blocks = barrier.getBlocks();
                    for (int k = 0; k < blocks.size(); k++) {
                        if (enemyBullets[i].getBounds().intersects(blocks[k].getGlobalBounds())) {
                            blocks.erase(blocks.begin() + k); // Düşman da bariyeri parçalar
                            eBulletDestroyed = true;
                            break;
                        }
                    }
                    if (eBulletDestroyed) break;
                }
            }

            if (eBulletDestroyed || enemyBullets[i].getY() > 600) {
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;
            }
        }
        }
        // 7. SEVİYE ATLATMA KONTROLÜ
        if (enemies.empty() && !isGameOver) {
            level++;
            levelText.setString("Seviye: " + std::to_string(level));

            // ZORLUK ARTIŞI
            swarmSpeed += 20.0f;         // Düşmanlar her seviyede daha hızlı kayar
            enemyShootInterval *= 0.9f;  // Ateş etme süresi %10 kısalır (daha sık ateş ederler)

            // Düşmanları yeniden oluştur (Aynı grid mantığı)
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    enemies.push_back(Enemy(startX + j * spacingX, startY + i * spacingY));
                }
            }

            // Mermileri temizle (Yeni seviyeye temiz bir ekranda başla)
            bullets.clear();
            enemyBullets.clear();
        }

        // --- OYUN GÜNCELLEME SONU ---

        // Ekranı temizle (Siyah renk)
        window.clear();
        
        player.draw(window); // Gemiyi çiz
        for (auto& bullet : bullets) {
            bullet.draw(window); // Aktif olan tüm mermileri çiz
            bullet.update(deltaTime);
        }
        for (auto& enemy : enemies) {
            enemy.draw(window);
        }
        for (auto& eBullet : enemyBullets) {
            eBullet.draw(window);
        }
        for (auto& barrier : barriers) {
            barrier.draw(window);
        }

        if (isGameOver && hasGameOverFont) {
            window.draw(gameOverText);
        }

        // UI elemanlarını çizdir (Oyun bitse bile ekranda kalsınlar)
        window.draw(scoreText);
        window.draw(livesText);
        window.draw(levelText);
        window.display();
    }
    
    


    return 0;
}