#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Barrier {
public:
    Barrier(float startX, float startY);
    void draw(sf::RenderWindow& window);
    std::vector<sf::RectangleShape>& getBlocks();
    std::vector<int>& getBlockHp();

private:
    std::vector<sf::RectangleShape> blocks;
    std::vector<int> blockHp;
};