#include "CoreRenderer.h"

#include "../Window.h"
#include "Core.h"

void CoreRenderer::update() {
    double distance = 25;
    const auto keyboard = this->getWindow()->getKeyboard();
    if (keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT])
        distance *= 4;
    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP])
        this->move(0, -distance);
    if (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN])
        this->move(0, distance);
    if (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT])
        this->move(-distance, 0);
    if (keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT])
        this->move(distance, 0);
}

bool CoreRenderer::handle(SDL_Event &event) {
    const auto keyboard = this->getWindow()->getKeyboard();
    switch (event.type) {
        case SDL_MOUSEWHEEL:
            if (keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT]) this->zoom(event.wheel.y * 4);
            else this->zoom(event.wheel.y);
            return true;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_EQUALS:
                    this->blockSize = 16;
                    this->calibrate();
                    return true;
                default:
                    break;
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.windowID == this->getWindow()->getWindowID()) {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        this->calibrate();
                        break;
                }
            }
            break;
    }
    return false;
}

CoreRenderer::CoreRenderer(Core &core, int blockSize) :
Component{0, 0, 1, 1},
core(core),
blockSize(blockSize) {}

void CoreRenderer::render(SDL_Renderer *renderer) {
    const double componentWidth = this->getComponentWidthP();
    const double componentHeight = this->getComponentHeightP();

    const auto xStart = static_cast<int>(this->startMapX);
    const auto yStart = static_cast<int>(this->startMapY);

    const auto xEnd = static_cast<int>(std::ceil(this->startMapX + componentWidth / this->blockSize));
    const auto yEnd = static_cast<int>(std::ceil(this->startMapY + componentHeight / this->blockSize));

    SDL_Rect rect;
    rect.w = this->blockSize;
    rect.h = this->blockSize;

    this->core.lockBuffer();
    for (int i = yStart; i < yEnd; ++i) {
        for (int j = xStart; j < xEnd; ++j) {
            if (!this->core.read(j, i)) continue;
            rect.x = static_cast<int>((static_cast<double>(j) - this->startMapX) * this->blockSize);
            rect.y = static_cast<int>((static_cast<double>(i) - this->startMapY) * this->blockSize);
            this->rectBuffer.draw(renderer, rect);
        }
    }
    this->core.unlockBuffer();
    this->rectBuffer.flush(renderer);

    if (this->getWindow()->getWindow() != this->getWindow()->getFocus()) return;

    const auto mapX = static_cast<int>(this->startMapX +
                                       (static_cast<double>(this->getWindow()->getMouseX()) / this->blockSize));
    const auto mapY = static_cast<int>(this->startMapY +
                                       (static_cast<double>(this->getWindow()->getMouseY()) / this->blockSize));

    rect.x = static_cast<int>((mapX - this->startMapX) * this->blockSize);
    rect.y = static_cast<int>((mapY - this->startMapY) * this->blockSize);

    SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void CoreRenderer::save(std::ofstream &stream) const {
    uint64_t size = this->blockSize;
    stream.write(reinterpret_cast<const char *>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char *>(&this->startMapX), sizeof(this->startMapX));
    stream.write(reinterpret_cast<const char *>(&this->startMapY), sizeof(this->startMapY));
}

void CoreRenderer::load(std::ifstream &stream) {
    uint64_t size;
    stream.read(reinterpret_cast<char *>(&size), sizeof(size));
    stream.read(reinterpret_cast<char *>(&this->startMapX), sizeof(this->startMapX));
    stream.read(reinterpret_cast<char *>(&this->startMapY), sizeof(this->startMapY));
    this->calibrate();
}

void CoreRenderer::calibrate() {
    const double componentWidth = this->getComponentWidthP();
    const double componentHeight = this->getComponentHeightP();
    const double coreWidth = this->core.getWidth();
    const double coreHeight = this->core.getHeight();

    if (this->blockSize < 1) this->blockSize = 1;

    if (componentWidth / this->blockSize > coreWidth)
        this->blockSize = static_cast<int>(std::ceil(componentWidth / this->core.getWidth()));
    if (componentHeight / this->blockSize > coreHeight)
        this->blockSize = static_cast<int>(std::ceil(componentHeight / this->core.getHeight()));

    if (this->startMapX < 0) this->startMapX = 0;
    else if (this->startMapX > (coreWidth - componentWidth / this->blockSize))
        this->startMapX = coreWidth - componentWidth / this->blockSize;
    if (this->startMapY < 0) this->startMapY = 0;
    else if (this->startMapY > (coreHeight - componentHeight / this->blockSize))
        this->startMapY = coreHeight - componentHeight / this->blockSize;
}

void CoreRenderer::move(double unitX, double unitY) {
    this->startMapX += unitX / this->blockSize;
    this->startMapY += unitY / this->blockSize;

    this->calibrate();
}

void CoreRenderer::zoom(int unitN) {
    const double mouseX = this->getWindow()->getMouseX();
    const double mouseY = this->getWindow()->getMouseY();

    auto newSize = this->blockSize + unitN;

    this->startMapX += mouseX / this->blockSize - mouseX / newSize;
    this->startMapY += mouseY / this->blockSize - mouseY / newSize;

    this->blockSize = newSize;

    this->calibrate();
}

int CoreRenderer::getBlockSize() const { return this->blockSize; }

double CoreRenderer::getStartMapX() const { return this->startMapX; }

double CoreRenderer::getStartMapY() const { return this->startMapY; }
