#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Barrier {
public:
    Barrier(float startX, float startY);
    void draw(sf::RenderWindow& window);
    std::vector<sf::RectangleShape>& getBlocks();

private:
    std::vector<sf::RectangleShape> blocks;
    // Her bloğun HP'si — renk için (3=yeşil 2=sarı 1=kırmızı 0=ölü)
    std::vector<int> blockHp;
};