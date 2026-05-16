# 🚀 Space Invaders: Modernized Arcade Experience

Bu proje, 1978 yapımı klasik **Space Invaders** oyununun ruhunu koruyarak, modern oyun tasarimi teknikleriyle (Game Juice) yeniden yorumlanmış bir versiyonudur. C++ ve SFML kütüphanesi kullanılarak geliştirilmiştir.

## ✨ Öne Çıkan Özellikler

Orijinal oyunun üzerine eklenen modern dokunuşlar:

*   **💓 Dinamik Kalp Atışı (Tempo Engine):** Düşman sürüsü azaldıkça hızlanan ve oyunun gerilimini artıran 4 notalı analog ritim motoru.
*   **🛸 Mothership (UFO) Sistemi:** Yenilenen metalik uçan daire (saucer) tasarımı, kırmızı neon zemin ışıması, sıralı yanıp sönen flaşörleri ve LFO modülasyonlu wawa sesiyle rastgele yüksek puan veren bonus gemi.
*   **🚀 Gelişmiş Oyuncu Gemisi:** Gövdeyle bütünleşik sleek kanat tasarımı ve hareket yönüne göre dinamik olarak püsküren sarı-turuncu egzoz motor alevi parçacıkları.
*   **🧱 Piksel Bazlı Parçalanan Siperler:** 5x5'lik piksellerden oluşan, hasar aldığında veya düşmanlar tarafından kemirildiğinde etrafa kırmızı siper kıvılcımları (sparks) saçarak gerçek zamanlı parçalanan siper mekaniği.
*   **🌌 3 Katmanlı Parallax Arkaplan:** Derinlik hissi veren, farklı hızlarda hareket eden yıldız sistemi.
*   **⚡ Geçici Güçlendirmeler (Power-up Capsules):** Düşmanlardan düşen kapsüllerle elde edilen özel yetenekler:
    *   **🛡️ Kalkan (Shield - S):** 7 saniye boyunca oyuncunun etrafında turkuaz renkli kalkan halkası oluşturur, ilk darbede kırılır ve can kaybını önler.
    *   **🔥 Hızlı Ateş (Rapid Fire - R):** 7 saniye boyunca ateş etme bekleme süresini (cooldown) yarıdan aza indirir.
    *   **⚡ Üçlü Atış (Triple Shot - T):** 7 saniye boyunca sol-açılı, merkez ve sağ-açılı olmak üzere aynı anda 3 yöne lazer atışı sağlar.
    *   **💣 Akıllı Bomba Cephanesi (Smart Bomb - B):** Alındığında oyuncuya fırlatılabilir 1 adet plazma bombası cephanesi verir.
*   **🎯 Fırlatılabilir Akıllı Bomba:** Mor plazma bombası fırlatıldıktan sonra bir düşmana, bariyere veya ekranın üst sınırına çarptığı koordinatta patlayarak **200 piksel yarıçapında mor-pembe bir şok dalgası** yaratır. Bu dalga etki alanındaki tüm düşman mermilerini siler, düşmanlara ve UFO'ya 1 HP hasar verir.
*   **📈 Nadirlik ve Dengeleme (Rarity & Probability):**
    *   **Düşme Oranları:** Normal düşmanlarda %5, Kamikazelerde %15, UFO vurulduğunda %50 ihtimalle kapsül düşer.
    *   **Ağırlıklı Dağılım:** Kapsül açıldığında Kalkan (%35), Üçlü Atış (%30), Hızlı Ateş (%25) veya çok güçlü olan Akıllı Bomba (%10) gelir.
*   **💥 Görsel Zenginlik (Juice):** 
    *   **Screen Shake:** Oyuncu vurulduğunda, UFO patladığında veya Akıllı Bomba patladığında oluşan ekran sarsıntısı.
    *   **Floating Score & Texts:** Vurulan düşmanlardan yükselen puanlar (+30) ve toplanan kapsüllerin isimleri (+KALKAN, +BOMBA CEPHANESI vb.).
    *   **Particle Explosions:** Her patlama için özel renkli parçacık efektleri.
*   **🔴 Kamikaze Saldırıları:** Kırmızı düşmanların (Tip C) formasyondan ayrılıp doğrudan oyuncunun üzerine intihar dalışı yapması ve siperlere çarptığında patlaması.

---

## 🎮 Kontroller

| Tuş | Eylem |
| :--- | :--- |
| **Sol / Sağ Ok** | Gemiyi Hareket Ettir |
| **Space (Boşluk)** | Standart Lazer Ateşle |
| **X** veya **Sol Shift (LShift)** | Fırlatılabilir Akıllı Bomba At (Cephane Varsa) |
| **R** | Oyunu Yeniden Başlat (Restart) |
| **P** | Oyunu Duraklat (Pause) |
| **M** | Sesi Aç / Kapat |
| **Enter (Menüde)** | Oyunu Başlat |
| **Esc** | Çıkış |

---

## 🛠️ Kurulum ve Derleme

### Gereksinimler
*   C++ Derleyici (C++17 destekli)
*   [SFML 2.5+](https://www.sfml-dev.org/)
*   CMake (3.10+)

### Derleme Adımları

Projeyi derlemek için aşağıdaki komutları sırasıyla terminalinizde çalıştırabilirsiniz:

1.  Projeyi klonlayın ve proje dizinine girin:
    ```bash
    cd SpaceInvaders_Project
    ```
2.  `build` klasörü oluşturup içine girin:
    ```bash
    mkdir build
    cd build
    ```
3.  CMake'i "Unix Makefiles" jeneratörü ile yapılandırın (MinGW veya Unix platformları için):
    ```bash
    cmake .. -G "Unix Makefiles"
    ```
4.  Release modunda projeyi derleyin:
    ```bash
    cmake --build . --config Release
    ```
5.  Oyunu çalıştırın:
    *   **Windows (MinGW/Makefiles):**
        ```powershell
        ./Release/SpaceInvaders.exe
        ```
    *   **Linux / macOS:**
        ```bash
        ./SpaceInvaders
        ```
