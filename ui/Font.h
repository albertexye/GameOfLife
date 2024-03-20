#pragma once

#include <SDL2/SDL.h>

class Font {
private:
    uint8_t r = 0, g = 0, b = 0, a = 0;
    int size = 1;
    int space = 0;
public:
    void render(SDL_Renderer *renderer, const char *text, int x, int y) const;

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void setSpace(int space);

    void setSize(int size);
};
