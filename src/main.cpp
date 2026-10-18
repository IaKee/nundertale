#include "core/Game.hpp"
#include <iostream>

int main(int argc, char** argv) {
    try {
        nundertale::Game game(argc, argv);
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
