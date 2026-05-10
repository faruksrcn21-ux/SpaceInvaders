#include "Barrier.h"

// HP → renk tablosu
static sf::Color hpColor(int hp) {
    if (hp >= 3) return sf::Color(0,   220, 80);   // yeşil
    if (hp == 2) return sf::Color(220, 200, 0);    // sarı
    return             sf::Color(220, 60,  0);     // turuncu-kırmızı
}

Barrier::Barrier(float startX, float startY) {
    // 4 sıra x 6 sütun blok — her biri 15x15
    // Üst 2 sıra biraz daha dar (kale dişi görünümü)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            // Kale dişi: üst köşe blokları atla
            if (i == 0 && (j == 0 || j == 5)) continue;

            sf::RectangleShape block(sf::Vector2f(15.0f, 15.0f));
            block.setFillColor(hpColor(3));
            block.setOutlineColor(sf::Color(0, 255, 120));
            block.setOutlineThickness(1.0f);
            block.setPosition(startX + j * 15.0f, startY + i * 15.0f);
            blocks.push_back(block);

            // Her blok için başlangıç HP = 3
            blockHp.push_back(3);
        }
    }
}

void Barrier::draw(sf::RenderWindow& window) {
    for (int i = 0; i < (int)blocks.size(); i++) {
        if (blockHp[i] > 0) {
            // Rengi HP'ye göre güncelle
            blocks[i].setFillColor(hpColor(blockHp[i]));
            window.draw(blocks[i]);
        }
    }
}

std::vector<sf::RectangleShape>& Barrier::getBlocks() {
    return blocks;
}