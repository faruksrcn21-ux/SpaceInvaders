# 🚀 Space Invaders: Modernized Arcade Experience

Bu proje, 1978 yapımı klasik **Space Invaders** oyununun ruhunu koruyarak, modern oyun tasarımı teknikleriyle (Game Juice) yeniden yorumlanmış bir versiyonudur. C++ ve SFML kütüphanesi kullanılarak geliştirilmiştir.

## ✨ Öne Çıkan Özellikler

Orijinal oyunun üzerine eklenen modern dokunuşlar:

*   **💓 Dinamik Kalp Atışı (Tempo Engine):** Düşman sürüsü azaldıkça hızlanan ve oyunun gerilimini artıran 4 notalı analog ritim motoru.
*   **🛸 Mothership (UFO) Sistemi:** Yenilenen metalik uçan daire (saucer) tasarımı, kırmızı neon zemin ışıması, sıralı yanıp sönen flaşörleri ve LFO modülasyonlu wawa sesiyle rastgele yüksek puan veren bonus gemi.
*   **🚀 Gelişmiş Oyuncu Gemisi:** Gövdeyle bütünleşik sleek kanat tasarımı ve hareket yönüne göre dinamik olarak püsküren sarı-turuncu egzoz motor alevi parçacıkları.
*   **🧱 Piksel Bazlı Parçalanan Siperler:** 5x5'lik piksellerden oluşan, hasar aldığında veya düşmanlar tarafından kemirildiğinde etrafa kırmızı siper kıvılcımları (sparks) saçarak gerçek zamanlı parçalanan siper mekaniği.
*   **🌌 3 Katmanlı Parallax Arkaplan:** Derinlik hissi veren, farklı hızlarda hareket eden yıldız sistemi.
*   **💥 Görsel Zenginlik (Juice):** 
    *   **Screen Shake:** Oyuncu vurulduğunda veya UFO patladığında oluşan ekran sarsıntısı.
    *   **Floating Score:** Vurulan düşmanlardan yükselen ve sönen puan yazıları.
    *   **Particle Explosions:** Her patlama için özel renkli parçacık efektleri.
*   **🔴 Kamikaze Saldırıları:** Kırmızı düşmanların (Tip C) formasyondan ayrılıp doğrudan oyuncunun üzerine intihar dalışı yapması ve siperlere çarptığında patlaması.

## 🎮 Kontroller

| Tuş | Eylem |
| :--- | :--- |
| **Sol / Sağ Ok** | Gemiyi Hareket Ettir |
| **Space (Boşluk)** | Ateş Et |
| **R** | Oyunu Yeniden Başlat (Restart) |
| **P** | Oyunu Duraklat (Pause) |
| **M** | Sesi Aç / Kapat |
| **Enter (Menüde)** | Oyunu Başlat |
| **Esc** | Çıkış |

## 🛠️ Kurulum ve Derleme

### Gereksinimler
*   C++ Derleyici (C++17 destekli)
*   [SFML 2.5+](https://www.sfml-dev.org/)
*   CMake (3.10+)

### Derleme Adımları
1.  Projeyi klonlayın ve proje dizinine girin: `cd SpaceInvaders_Project`
2.  `build` klasörü oluşturup içine girin: `mkdir build && cd build`
3.  CMake'i "Unix Makefiles" jeneratörü ile yapılandırın:
    ```bash
    cmake .. -G "Unix Makefiles"
    ```
4.  Release modunda derleyin:
    ```bash
    cmake --build . --config Release
    ```
5.  Oyunu çalıştırın:
    *   **Windows (MinGW/Makefiles):** `./SpaceInvaders.exe`
    *   **Linux / macOS:** `./SpaceInvaders`
