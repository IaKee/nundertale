#include "core/Game.hpp"
#include <iostream>

int main() {
    try {
        nundertale::Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
