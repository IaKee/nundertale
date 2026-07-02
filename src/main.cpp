#include "core/Game.hpp"
#include "core/CrashHandler.hpp"
#include <iostream>

int main(int argc, char** argv) {
    nundertale::installCrashHandler();
    try {
        nundertale::Game game(argc, argv);
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
