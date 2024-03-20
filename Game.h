#pragma once

#include "Window.h"
#include "core/Core.h"
#include "ui/GPS.h"

class Game {
private:
    std::shared_ptr<Core> core;
    std::shared_ptr<CoreRenderer> coreRenderer;
    std::shared_ptr<CoreExtension> coreExtension;
    std::shared_ptr<CoreSelector> coreSelector;

    std::shared_ptr<GPS> gps;

    Window window;

public:
    Game(int coreWidth, int coreHeight, int coreSpeed, int coreThread, int zoom, int windowWidth, int windowHeight);

    [[noreturn]] void run();
};
