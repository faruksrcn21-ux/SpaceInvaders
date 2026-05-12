#pragma once
#include <SFML/Audio.hpp>
#include <array>

// Ses sistemi — SoundManager
//
// Harici .wav/.ogg dosyası gerektirmez.
// Tüm sesler kod içinde sinüs/kare dalga üretilerek oluşturulur
// ve sf::SoundBuffer'a yazılır.
//
// Desteklenen sesler:
//   playShoot()      — oyuncu lazer atışı (kısa, yüksek frekanslı)
//   playEnemyShoot() — düşman plazma atışı (kısa, düşük frekanslı)
//   playExplosion()  — patlama (gürültü patlaması)
//   playPlayerHit()  — oyuncu hasar (düşük düz ton)
//   startMusic()     — retro arkaplan müziği döngüsü
//   stopMusic()      — müziği durdur


class SoundManager {
public:
    SoundManager();

    void playShoot();
    void playEnemyShoot();
    void playExplosion();
    void playPlayerHit();
    void playUfo();
    void stopUfo();
    void playFleetStep();
    void startMusic();
    void stopMusic();
    void toggleMute();
    bool isMuted() const;

private:
    // Ses üretim yardımcıları
    void buildShootBuffer();
    void buildEnemyShootBuffer();
    void buildExplosionBuffer();
    void buildPlayerHitBuffer();
    void buildUfoBuffer();
    void buildFleetBuffers();
    void buildMusicBuffer();

    // Her ses için ayrı buffer + sound çifti
    sf::SoundBuffer shootBuf_;
    sf::SoundBuffer enemyShootBuf_;
    sf::SoundBuffer explosionBuf_;
    sf::SoundBuffer playerHitBuf_;
    sf::SoundBuffer ufoBuf_;
    sf::SoundBuffer fleetBuf_[4];
    sf::SoundBuffer musicBuf_;

    sf::Sound shootSnd_;
    sf::Sound enemyShootSnd_;
    sf::Sound explosionSnd_;
    sf::Sound playerHitSnd_;
    sf::Sound ufoSnd_;
    sf::Sound fleetSnd_[4];
    int fleetStep_ = 0;
    sf::Sound musicSnd_;          // döngülü arkaplan müziği
    bool muted_ = false;           // ses kapalı mı?

    // Sinyal üretim yardımcısı — PCM örneklerini doldurur
    static void sineWave(std::vector<sf::Int16>& samples,
                         unsigned sampleRate,
                         float durationSec,
                         float startFreq,
                         float endFreq,      // frekans sweep için
                         float amplitude);

    static void noiseWave(std::vector<sf::Int16>& samples,
                          unsigned sampleRate,
                          float durationSec,
                          float amplitude,
                          float decayRate);  // zaman içinde azalsın
};