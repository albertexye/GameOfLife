#include "CoreSelector.h"

#include "../Window.h"
#include "Core.h"
#include "CoreRenderer.h"
#include "CoreExtension.h"

void CoreSelector::swap(int &xStart, int &xEnd, int &yStart, int &yEnd) const {
    if (this->startMapX < this->endMapX) {
        xStart = this->startMapX;
        xEnd = this->endMapX;
    } else {
        xStart = this->endMapX;
        xEnd = this->startMapX;
    }
    if (this->startMapY < this->endMapY) {
        yStart = this->startMapY;
        yEnd = this->endMapY;
    } else {
        yStart = this->endMapY;
        yEnd = this->startMapY;
    }
}

bool CoreSelector::handle(SDL_Event &event) {
    const auto keyboard = this->getWindow()->getKeyboard();
    const auto mouseState = this->getWindow()->getMouseState();

    switch (event.type) {
        case SDL_MOUSEMOTION:
            if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                this->toggle();
                return true;
            }
            if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                this->updateSelect();
                return true;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    this->clearSelect();
                    return true;
                case SDL_BUTTON_RIGHT:
                    this->startSelect();
                    return true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    this->clearToggle();
                    return true;
                case SDL_BUTTON_RIGHT:
                    this->endSelect();
                    return true;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case 'c':
                    if (keyboard[SDL_SCANCODE_LCTRL] || keyboard[SDL_SCANCODE_RCTRL]) {
                        this->copy();
                        return true;
                    }
                    break;
                case 'x':
                    if (keyboard[SDL_SCANCODE_LCTRL] || keyboard[SDL_SCANCODE_RCTRL]) {
                        this->cut();
                        return true;
                    }
                    break;
                case 'v':
                    if (keyboard[SDL_SCANCODE_LCTRL] || keyboard[SDL_SCANCODE_RCTRL]) {
                        this->paste();
                        return true;
                    }
                    break;
                case 'f':
                    if (keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT]) this->empty();
                    else this->fill();
                    return true;
                case 'r':
                    this->inverse();
                    return true;
            }
            break;
    }
    return false;
}

void CoreSelector::render(SDL_Renderer *renderer) {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    const auto mapX = this->coreRenderer.getStartMapX();
    const auto mapY = this->coreRenderer.getStartMapY();
    const double componentWidth = this->getComponentWidthP();
    const double componentHeight = this->getComponentHeightP();
    const auto blockSize = this->coreRenderer.getBlockSize();

    SDL_Rect rect;
    rect.w = blockSize;
    rect.h = blockSize;

    for (int i = yStart; i <= yEnd; ++i) {
        if (i < static_cast<int>(mapY) || i > static_cast<int>(mapY + componentHeight / blockSize)) continue;
        for (int j = xStart; j <= xEnd; ++j) {
            if (j < static_cast<int>(mapX) || j > static_cast<int>(mapX + componentWidth / blockSize)) continue;
            rect.x = static_cast<int>((static_cast<double>(j) - mapX) * blockSize);
            rect.y = static_cast<int>((static_cast<double>(i) - mapY) * blockSize);
            if (this->core.read(j, i)) this->rectLiveBuffer.draw(renderer, rect);
            else this->rectDeadBuffer.draw(renderer, rect);
        }
    }

    this->rectLiveBuffer.flush(renderer);
    this->rectDeadBuffer.flush(renderer);

    if (!this->updating) {
        rect.x = xStart;
        rect.y = yStart;
        rect.w = xEnd - xStart;
        rect.h = yEnd - yStart;
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void CoreSelector::save(std::ofstream &stream) const {
    uint32_t startX = this->startMapX;
    uint32_t startY = this->startMapY;
    uint32_t endX = this->endMapX;
    uint32_t endY = this->endMapY;

    stream.put(static_cast<char>(this->selecting));

    stream.write(reinterpret_cast<const char *>(&startX), sizeof(startX));
    stream.write(reinterpret_cast<const char *>(&startY), sizeof(startY));
    stream.write(reinterpret_cast<const char *>(&endX), sizeof(endX));
    stream.write(reinterpret_cast<const char *>(&endY), sizeof(endY));
}

void CoreSelector::load(std::ifstream &stream) {
    uint32_t startX, startY, endX, endY;

    this->selecting = stream.get();

    stream.read(reinterpret_cast<char *>(&startX), sizeof(startX));
    stream.read(reinterpret_cast<char *>(&startY), sizeof(startY));
    stream.read(reinterpret_cast<char *>(&endX), sizeof(endX));
    stream.read(reinterpret_cast<char *>(&endY), sizeof(endY));

    this->startMapX = static_cast<int>(startX);
    this->startMapY = static_cast<int>(startY);
    this->endMapX = static_cast<int>(endX);
    this->endMapY = static_cast<int>(endY);
}

CoreSelector::CoreSelector(Core &core, CoreRenderer &coreRenderer, CoreExtension &coreExtension) :
Component(0, 0, 1, 1),
core(core),
coreRenderer(coreRenderer),
coreExtension(coreExtension)
{
    this->rectLiveBuffer.setColor(255, 0, 0, 255);
    this->rectDeadBuffer.setColor(0, 255, 0, 255);
}

void CoreSelector::startSelect() {
    if (this->updating) return;
    const auto blockSize = this->coreRenderer.getBlockSize();
    this->startMapX = static_cast<int>(this->coreRenderer.getStartMapX() +
                                       static_cast<double>(this->getWindow()->getMouseX()) / blockSize);
    this->startMapY = static_cast<int>(this->coreRenderer.getStartMapY() +
                                       static_cast<double>(this->getWindow()->getMouseY()) / blockSize);
    this->endMapX = this->startMapX;
    this->endMapY = this->startMapY;

    this->updating = true;
    this->selecting = true;
}

void CoreSelector::updateSelect() {
    if (!this->updating) return;
    const auto blockSize = this->coreRenderer.getBlockSize();
    this->endMapX = static_cast<int>(this->coreRenderer.getStartMapX() +
                                     static_cast<double>(this->getWindow()->getMouseX()) / blockSize);
    this->endMapY = static_cast<int>(this->coreRenderer.getStartMapY() +
                                     static_cast<double>(this->getWindow()->getMouseY()) / blockSize);
}

void CoreSelector::endSelect() { this->updating = false; }

void CoreSelector::clearSelect() { this->selecting = false; }

void CoreSelector::toggle() {
    const auto mouseX = this->getWindow()->getMouseX();
    const auto mouseY = this->getWindow()->getMouseY();

    const auto blockSize = this->coreRenderer.getBlockSize();
    const auto mapX = static_cast<int>(this->coreRenderer.getStartMapX() + static_cast<double>(mouseX) / blockSize);
    const auto mapY = static_cast<int>(this->coreRenderer.getStartMapY() + static_cast<double>(mouseY) / blockSize);

    if (mapX == this->lastMapX && mapY == this->lastMapY) return;

    this->lastMapX = mapX;
    this->lastMapY = mapY;
    this->core.set(mapX, mapY, !this->core.read(mapX, mapY));
}

void CoreSelector::clearToggle() {
    this->lastMapX = -1;
    this->lastMapY = -1;
}

void CoreSelector::copy() const {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    this->coreExtension.copy(xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1);
}

void CoreSelector::cut() {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    this->coreExtension.cut(xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1);
}

void CoreSelector::paste() {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    this->coreExtension.paste(xStart, yStart);
}

void CoreSelector::fill() {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    this->coreExtension.fill(xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1);
}

void CoreSelector::empty() {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    this->coreExtension.empty(xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1);
}

void CoreSelector::inverse() {
    if (!this->selecting) return;

    int xStart, xEnd, yStart, yEnd;
    this->swap(xStart, xEnd, yStart, yEnd);

    this->coreExtension.inverse(xStart, yStart, xEnd - xStart + 1, yEnd - yStart + 1);
}
