#pragma once

#include <initializer_list>

#include <SDL2/SDL.h>

#include "../RectBuffer.h"

class Character {
private:
    uint8_t map[7][5]{};
    RectBuffer rectBuffer{0};

public:
    Character(std::initializer_list<uint8_t> map);

    void render(SDL_Renderer *renderer, int x, int y, int size);

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};
