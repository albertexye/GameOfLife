#include "Character.h"

Character::Character(std::initializer_list<uint8_t> map) {
    if (map.size() != 35) throw;

    std::copy(map.begin(), map.end(), *this->map);

    int count{0};
    for (const auto &row : this->map) {
        for (const auto &pixel : row) {
            count += pixel;
        }
    }
    this->rectBuffer.resize(count);
}

void Character::render(SDL_Renderer *renderer, int x, int y, int size) {
    SDL_Rect rect;
    rect.w = size;
    rect.h = size;
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 5; j++) {
            if (this->map[i][j]) {
                rect.x = x + j * size;
                rect.y = y + i * size;
                this->rectBuffer.draw(renderer, rect);
            }
        }
    }
    this->rectBuffer.flush(renderer);
}

void Character::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    this->rectBuffer.setColor(r, g, b, a);
}
