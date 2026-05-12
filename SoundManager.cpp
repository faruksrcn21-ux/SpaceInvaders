#include "SoundManager.h"
#include <cmath>
#include <cstdlib>
#include <vector>

static const float PI = 3.14159265f;
static const unsigned SAMPLE_RATE = 44100;

// ── Sinyal üretim yardımcıları ────────────────────────────────────────────

// Frekansı startFreq'ten endFreq'e sweep eden sinüs dalgası
void SoundManager::sineWave(std::vector<sf::Int16>& samples,
                             unsigned sampleRate,
                             float durationSec,
                             float startFreq,
                             float endFreq,
                             float amplitude)
{
    int count = static_cast<int>(sampleRate * durationSec);
    samples.resize(count);
    for (int i = 0; i < count; i++) {
        float t    = static_cast<float>(i) / sampleRate;
        float prog = static_cast<float>(i) / count;
        float freq = startFreq + (endFreq - startFreq) * prog;
        // Zarf: başta ve sonda kısa fade
        float env  = std::min(t / 0.005f, 1.f) *
                     std::min((durationSec - t) / 0.01f, 1.f);
        samples[i] = static_cast<sf::Int16>(
            std::sin(2.f * PI * freq * t) * amplitude * env * 32767.f);
    }
}

// Beyaz gürültü + üstel bozunma (patlama için)
void SoundManager::noiseWave(std::vector<sf::Int16>& samples,
                              unsigned sampleRate,
                              float durationSec,
                              float amplitude,
                              float decayRate)
{
    int count = static_cast<int>(sampleRate * durationSec);
    samples.resize(count);
    for (int i = 0; i < count; i++) {
        float t     = static_cast<float>(i) / sampleRate;
        float decay = std::exp(-decayRate * t);
        float noise = ((float)rand() / RAND_MAX * 2.f - 1.f);
        samples[i]  = static_cast<sf::Int16>(noise * amplitude * decay * 32767.f);
    }
}

// ── Buffer inşa fonksiyonları ─────────────────────────────────────────────

// Oyuncu lazeri: 880Hz → 440Hz sweep, 0.12 sn
void SoundManager::buildShootBuffer() {
    std::vector<sf::Int16> s;
    sineWave(s, SAMPLE_RATE, 0.12f, 880.f, 440.f, 0.4f);
    shootBuf_.loadFromSamples(s.data(), s.size(), 1, SAMPLE_RATE);
}

// Düşman plazması: 220Hz → 110Hz, 0.18 sn, daha kalın
void SoundManager::buildEnemyShootBuffer() {
    std::vector<sf::Int16> s;
    sineWave(s, SAMPLE_RATE, 0.18f, 220.f, 110.f, 0.35f);
    enemyShootBuf_.loadFromSamples(s.data(), s.size(), 1, SAMPLE_RATE);
}

// Patlama: beyaz gürültü + hızlı bozunma, 0.4 sn
void SoundManager::buildExplosionBuffer() {
    std::vector<sf::Int16> s;
    noiseWave(s, SAMPLE_RATE, 0.4f, 0.8f, 12.f);
    explosionBuf_.loadFromSamples(s.data(), s.size(), 1, SAMPLE_RATE);
}

// Oyuncu hasar: alçak düz ton + gürültü karışımı, 0.3 sn
void SoundManager::buildPlayerHitBuffer() {
    std::vector<sf::Int16> sNoise, sTone;
    noiseWave(sNoise, SAMPLE_RATE, 0.3f, 0.5f, 8.f);
    sineWave(sTone,  SAMPLE_RATE, 0.3f, 150.f, 80.f, 0.4f);
    std::vector<sf::Int16> s(sNoise.size());
    for (int i = 0; i < (int)s.size(); i++) {
        int mixed = static_cast<int>(sNoise[i]) + static_cast<int>(sTone[i]);
        // Clamp: -32768..32767
        if (mixed >  32767) mixed =  32767;
        if (mixed < -32768) mixed = -32768;
        s[i] = static_cast<sf::Int16>(mixed);
    }
    playerHitBuf_.loadFromSamples(s.data(), s.size(), 1, SAMPLE_RATE);
}

// UFO sesi: frekansı LFO ile dalgalanan sinüs (wawa efekti), 1 saniyelik döngü
void SoundManager::buildUfoBuffer() {
    float durationSec = 1.0f;
    int count = static_cast<int>(SAMPLE_RATE * durationSec);
    std::vector<sf::Int16> s(count);

    float carrierFreq = 300.f;  // taşıyıcı frekans
    float modFreq     = 8.f;    // 1 saniyede 8 kez dalgalanır
    float modAmount   = 100.f;  // 300 +/- 100 Hz arası
    float phase = 0.f;

    for (int i = 0; i < count; i++) {
        float t = static_cast<float>(i) / SAMPLE_RATE;
        float currentFreq = carrierFreq + std::sin(2.f * PI * modFreq * t) * modAmount;
        phase += 2.f * PI * currentFreq / SAMPLE_RATE;
        s[i] = static_cast<sf::Int16>(std::sin(phase) * 0.3f * 32767.f);
    }
    ufoBuf_.loadFromSamples(s.data(), s.size(), 1, SAMPLE_RATE);
}

// Retro arkaplan müziği: basit 8-bit tarzı arpej dizisi
// 4 nota → döngülü tekrar (sf::Sound::setLoop kullanıyoruz)
void SoundManager::buildMusicBuffer() {
    // Arpeji oluşturan notalar (Hz)
    // C4=261, E4=329, G4=392, B4=493, C5=523
    const float notes[] = { 261.f, 329.f, 392.f, 493.f,
                             392.f, 329.f, 261.f, 196.f };
    const int   NOTE_COUNT = 8;
    const float NOTE_DUR   = 0.12f;  // her nota süresi

    std::vector<sf::Int16> allSamples;
    allSamples.reserve(static_cast<int>(SAMPLE_RATE * NOTE_DUR * NOTE_COUNT));

    for (int n = 0; n < NOTE_COUNT; n++) {
        std::vector<sf::Int16> noteSamples;
        // Her nota kısa sweep: notanın kendisinden biraz aşağıya
        sineWave(noteSamples, SAMPLE_RATE, NOTE_DUR,
                 notes[n], notes[n] * 0.98f, 0.18f);
        allSamples.insert(allSamples.end(),
                          noteSamples.begin(), noteSamples.end());
    }

    musicBuf_.loadFromSamples(allSamples.data(), allSamples.size(),
                               1, SAMPLE_RATE);
}

// ── Constructor ───────────────────────────────────────────────────────────

SoundManager::SoundManager() {
    buildShootBuffer();
    buildEnemyShootBuffer();
    buildExplosionBuffer();
    buildPlayerHitBuffer();
    buildUfoBuffer();
    buildMusicBuffer();

    // Soundları buffer'lara bağla
    shootSnd_.setBuffer(shootBuf_);
    enemyShootSnd_.setBuffer(enemyShootBuf_);
    explosionSnd_.setBuffer(explosionBuf_);
    playerHitSnd_.setBuffer(playerHitBuf_);
    ufoSnd_.setBuffer(ufoBuf_);
    musicSnd_.setBuffer(musicBuf_);

    // UFO döngülü çalar
    ufoSnd_.setLoop(true);
    ufoSnd_.setVolume(60.f);

    // Arkaplan müziği döngülü
    musicSnd_.setLoop(true);
    musicSnd_.setVolume(35.f);  // diğer sesler boğulmasın

    // Efektler biraz daha yüksek
    shootSnd_.setVolume(70.f);
    enemyShootSnd_.setVolume(60.f);
    explosionSnd_.setVolume(85.f);
    playerHitSnd_.setVolume(90.f);
}

// ── Public API ────────────────────────────────────────────────────────────

void SoundManager::playShoot() {
    if (muted_) return;
    shootSnd_.play();
}

void SoundManager::playEnemyShoot() {
    if (muted_) return;
    enemyShootSnd_.play();
}

void SoundManager::playExplosion() {
    if (muted_) return;
    explosionSnd_.stop();
    explosionSnd_.play();
}

void SoundManager::playPlayerHit() {
    if (muted_) return;
    playerHitSnd_.stop();
    playerHitSnd_.play();
}

void SoundManager::playUfo() {
    if (muted_) return;
    if (ufoSnd_.getStatus() != sf::Sound::Playing)
        ufoSnd_.play();
}

void SoundManager::stopUfo() {
    ufoSnd_.stop();
}

void SoundManager::startMusic() {
    if (musicSnd_.getStatus() != sf::Sound::Playing)
        musicSnd_.play();
}

void SoundManager::stopMusic() {
    musicSnd_.stop();
}

void SoundManager::toggleMute() {
    muted_ = !muted_;
    if (muted_) {
        musicSnd_.pause();
        ufoSnd_.pause();
    } else {
        musicSnd_.play();
        // UFO ancak aktifse GameManager tarafından playUfo() ile başlatılır
    }
}

bool SoundManager::isMuted() const {
    return muted_;
}