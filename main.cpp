#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#include "Game.h"

[[noreturn]] int main(int argv, char **args) {
    Game game{32, 32, 4, 4, 16, 512, 512};
    game.run();
}
