#include "RectBuffer.h"

RectBuffer::RectBuffer(int size) {
    this->rectBuffer.resize(size);
}

void RectBuffer::draw(SDL_Renderer *renderer, const SDL_Rect &rect) {
    this->rectBuffer[this->rectCount] = rect;
    if (++this->rectCount != this->rectBuffer.size()) return;
    this->rectCount = 0;
    SDL_SetRenderDrawColor(renderer, this->r, this->g, this->b, this->a);
    SDL_RenderFillRects(renderer, this->rectBuffer.data(), static_cast<int>(this->rectBuffer.size()));
}

void RectBuffer::flush(SDL_Renderer *renderer) {
    if (this->rectCount == 0) return;
    SDL_SetRenderDrawColor(renderer, this->r, this->g, this->b, this->a);
    SDL_RenderFillRects(renderer, this->rectBuffer.data(), this->rectCount);
    this->rectCount = 0;
}

void RectBuffer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void RectBuffer::resize(int size) {
    this->rectBuffer.resize(size);
}
