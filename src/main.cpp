
#include "oop/oopgame.h"
#include "ecs/ecsgame.h"
#include <iostream>

int main() {
    //Game game;
    Ecs::Game game;

    if (game.init()) {
        game.run();
    } else {
        std::cerr << "Failed to initialize game." << std::endl;
        return -1;
    }
    return 0;
}
