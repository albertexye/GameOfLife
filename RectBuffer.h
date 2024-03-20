#pragma once

#include <vector>

#include <SDL2/SDL.h>

class RectBuffer {
private:
    std::vector<SDL_Rect> rectBuffer{};
    int rectCount{0};
    uint8_t r{255}, g{255}, b{255}, a{255};

public:
    explicit RectBuffer(int size);

    void draw(SDL_Renderer *renderer, const SDL_Rect &rect);

    void flush(SDL_Renderer *renderer);

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void resize(int size);
};
