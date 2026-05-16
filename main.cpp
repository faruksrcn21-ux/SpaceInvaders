#include "GameManager.h"
#include <iostream>

int main() {
    try {
        GameManager game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "HATA: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "BILINMEYEN HATA!" << std::endl;
        return 1;
    }
    return 0;
}