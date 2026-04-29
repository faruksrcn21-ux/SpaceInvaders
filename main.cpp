#include <SFML/Graphics.hpp>

int main() {
    // 800x600 boyutunda bir oyun penceresi oluşturur
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");

    // Oyun döngüsü: Pencere açık olduğu sürece çalışır
    while (window.isOpen()) {
        sf::Event event;
        // Kapatma tuşuna basılıp basılmadığını kontrol eder
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Ekranı temizle (Siyah renk)
        window.clear();
        
        // Buraya ileride gemimizi ve düşmanlarımızı çizdireceğiz
        
        // Çizilenleri ekrana yansıt
        window.display();
    }

    return 0;
}