#include "Barrier.h"

Barrier::Barrier(float startX, float startY) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            sf::RectangleShape block(sf::Vector2f(15.0f, 15.0f));
            block.setFillColor(sf::Color::Cyan);
            block.setPosition(startX + j * 15.0f, startY + i * 15.0f);
            blocks.push_back(block);
        }
    }
}

void Barrier::draw(sf::RenderWindow& window) {
    for (auto& block : blocks) window.draw(block);
}

std::vector<sf::RectangleShape>& Barrier::getBlocks() { return blocks; }