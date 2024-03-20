#include "Font.h"

#include "characterSet.h"

void Font::render(SDL_Renderer *renderer, const char *text, int x, int y) const {
    const auto characterSize = this->size * 5 + this->space;
    int i = 0;
    SDL_SetRenderDrawColor(renderer, this->r, this->g, this->b, this->a);
    for (char c = *text; c != '\0'; ++i) {
        if (c >= 32 && c <= 126) {
            auto &character = CharacterSet[c - 32];
            character.setColor(this->r, this->g, this->b, this->a);
            character.render(renderer, x + characterSize * i, y, this->size);
        } else {
            auto &character = CharacterSet[95];
            character.setColor(this->r, this->g, this->b, this->a);
            character.render(renderer, x + characterSize * i, y, this->size);
        }
        c = *(++text);
    }
}

void Font::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Font::setSpace(int space) {
    this->space = space;
}

void Font::setSize(int size) {
    this->size = size;
}
