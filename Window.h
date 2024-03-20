#pragma once

#include <cinttypes>
#include <vector>
#include <memory>

#include <SDL2/SDL.h>

#include "Component.h"

class Window {
private:
    SDL_Window *window{};
    SDL_Renderer *renderer{};

    std::vector<std::shared_ptr<Component>> components{};

    int mouseX = 0, mouseY = 0, windowWidth = 0, windowHeight = 0;
    int lastPosX = 0, lastPosY = 0, lastWidth = 0, lastHeight = 0;
    SDL_Window *focus{};
    uint32_t mouseState = 0;
    uint32_t windowID{};
    const uint8_t *keyboard{};

    void toggleFullScreen();

    static int filterEvent(void *userdata, SDL_Event *event);

public:
    Window(int width, int height, std::initializer_list<std::shared_ptr<Component>> components);

    ~Window();

    void update();

    void handle();

    void render();

    void save() const;

    void load();

    [[nodiscard]] int getMouseX() const;

    [[nodiscard]] int getMouseY() const;

    [[nodiscard]] uint32_t getMouseState() const;

    [[nodiscard]] int getWindowWidth() const;

    [[nodiscard]] int getWindowHeight() const;

    [[nodiscard]] const uint8_t *getKeyboard() const;

    [[nodiscard]] const SDL_Window *getFocus() const;

    [[nodiscard]] const SDL_Window *getWindow() const;

    [[nodiscard]] uint32_t getWindowID() const;
};
