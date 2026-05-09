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
};