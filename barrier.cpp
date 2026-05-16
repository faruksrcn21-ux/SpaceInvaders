#include "Barrier.h"

// HP → renk tablosu
static sf::Color hpColor(int hp) {
    if (hp >= 3) return sf::Color(0,   220, 80);   // yeşil
    if (hp == 2) return sf::Color(220, 200, 0);    // sarı
    return             sf::Color(220, 60,  0);     // turuncu-kırmızı
}

Barrier::Barrier(float startX, float startY) {
    const int ROWS = 10;
    const int COLS = 13;
    const float PIXEL_SIZE = 5.0f;

    // Klasik Space Invaders kalkan formu (13x10'luk piksel haritası)
    const char* pattern[ROWS] = {
        "    XXXXX    ",
        "  XXXXXXXXX  ",
        " XXXXXXXXXXX ",
        "XXXXXXXXXXXXX",
        "XXXXXXXXXXXXX",
        "XXXXXXXXXXXXX",
        "XXXXXXXXXXXXX",
        "XXXXXXXXXXXXX",
        "XXXX     XXXX",
        "XXXX     XXXX"
    };

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (pattern[i][j] == 'X') {
                sf::RectangleShape block(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
                block.setFillColor(sf::Color(0, 220, 80)); // Klasik yeşil
                block.setPosition(startX + j * PIXEL_SIZE, startY + i * PIXEL_SIZE);
                
                blocks.push_back(block);
                blockHp.push_back(1); // Piksel bazlı: tek vuruşta yok olur
            }
        }
    }
}

void Barrier::draw(sf::RenderWindow& window) {
    for (int i = 0; i < (int)blocks.size(); i++) {
        if (blockHp[i] <= 0) continue;         // ölü blok çizilmez
        blocks[i].setFillColor(hpColor(blockHp[i]));
        window.draw(blocks[i]);
    }
}

std::vector<sf::RectangleShape>& Barrier::getBlocks() {
    return blocks;
}

std::vector<int>& Barrier::getBlockHp() {
    return blockHp;
}