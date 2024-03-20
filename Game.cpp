#include "Game.h"

Game::Game(int coreWidth, int coreHeight, int coreSpeed, int coreThread, int zoom, int windowWidth, int windowHeight)
        : core(std::make_shared<Core>(coreWidth, coreHeight, coreSpeed, coreThread)),
          coreExtension(std::make_shared<CoreExtension>(*core)),
          coreRenderer(std::make_shared<CoreRenderer>(*core, zoom)),
          coreSelector(std::make_shared<CoreSelector>(*core, *coreRenderer, *coreExtension)),
          gps(std::make_shared<GPS>(*core)),
          window(windowWidth, windowHeight, {core, coreRenderer, coreSelector, gps}) {
    while (!this->core->getReady()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

[[noreturn]] void Game::run() {
    while (true) {
        this->window.update();
        this->window.handle();
        this->window.render();
    }
}
