#include "Window.h"

#include <ranges>

#include "tinyfiledialogs/tinyfiledialogs.h"

void Window::toggleFullScreen() {
    bool isFullscreen = SDL_GetWindowFlags(this->window) & SDL_WINDOW_FULLSCREEN;
    if (isFullscreen) {
        SDL_SetWindowFullscreen(this->window, 0);
        SDL_SetWindowSize(this->window, this->lastWidth, this->lastHeight);
        SDL_SetWindowPosition(this->window, this->lastPosX, this->lastPosY);

        this->windowWidth = this->lastWidth;
        this->windowHeight = this->lastHeight;
    } else {
        SDL_GetWindowSize(this->window, &this->lastWidth, &this->lastHeight);
        SDL_GetWindowPosition(this->window, &this->lastPosX, &this->lastPosY);

        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);
        const auto screenWidth = displayMode.w;
        const auto screenHeight = displayMode.h;
        this->windowWidth = screenWidth;
        this->windowHeight = screenHeight;

        SDL_SetWindowSize(this->window, screenWidth, screenHeight);
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN);
    }
}

int Window::filterEvent(void *userdata, SDL_Event *event) {
    auto thisPtr = reinterpret_cast<Window *>(userdata);
    if (event->type == SDL_WINDOWEVENT && thisPtr->windowID == event->window.windowID &&
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
        SDL_GetWindowSize(thisPtr->window, &thisPtr->windowWidth, &thisPtr->windowHeight);
        thisPtr->update();
        thisPtr->render();
        return 0;
    }
    return 1;
}

Window::Window(int width, int height, std::initializer_list<std::shared_ptr<Component>> components) : windowWidth(
        width), windowHeight(height), components(components) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);

    this->window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                    SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!this->window) exit(1);

    SDL_SetEventFilter(Window::filterEvent, this);

    this->windowID = SDL_GetWindowID(this->window);

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!this->renderer) exit(1);

    for (auto &component: components) {
        component->setWindow(this);
    }
}

Window::~Window() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Window::update() {
    this->mouseState = SDL_GetMouseState(&this->mouseX, &this->mouseY);

    this->focus = SDL_GetMouseFocus();

    this->keyboard = SDL_GetKeyboardState(nullptr);

    for (auto &component: this->components) {
        component->update();
    }
}

void Window::handle() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_F11:
                        this->toggleFullScreen();
                        continue;
                    case SDLK_ESCAPE:
                        exit(0);
                    case 'o':
                        if (this->keyboard[SDL_SCANCODE_LCTRL] || this->keyboard[SDL_SCANCODE_RCTRL]) {
                            this->load();
                            continue;
                        }
                        break;
                    case 's':
                        if (this->keyboard[SDL_SCANCODE_LCTRL] || this->keyboard[SDL_SCANCODE_RCTRL]) {
                            this->save();
                            continue;
                        }
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    SDL_GetWindowSize(this->window, &this->windowWidth, &this->windowHeight);
                    continue;
                }
                break;
        }
        for (auto &component: std::ranges::views::reverse(this->components)) {
            if (event.type == SDL_MOUSEWHEEL ||
            event.type == SDL_MOUSEBUTTONUP ||
            event.type == SDL_MOUSEBUTTONDOWN ||
            event.type == SDL_MOUSEMOTION) {
                if (this->mouseX < component->getComponentXP() || this->mouseX >
                                                                 component->getComponentXP() +
                                                                         component->getComponentWidthP() ||
                this->mouseY < component->getComponentYP() || this->mouseY >
                                                             component->getComponentYP() +
                                                                     component->getComponentHeightP())
                    continue;
            }
            if (component->handle(event)) break;
        }
    }
}

void Window::render() {
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderClear(this->renderer);

    for (auto &component: this->components) {
        component->render(this->renderer);
    }

    SDL_RenderPresent(this->renderer);
}

void Window::save() const {
    const char *pattern = "*.gol";
    const auto filename = tinyfd_saveFileDialog("Save Game", "game", 1, &pattern, "Game of Life file");
    if (filename == nullptr) return;

    std::ofstream stream{filename, std::ios::out | std::ios::binary | std::ios::trunc};

    for (auto &component: this->components) {
        component->save(stream);
    }
}

void Window::load() {
    const char *pattern = "*.gol";
    const auto filename = tinyfd_openFileDialog("Open Game", "", 1, &pattern, "Game of Life file", false);
    if (filename == nullptr) return;

    std::ifstream stream{filename, std::ios::in | std::ios::binary};

    for (auto &component: this->components) {
        component->load(stream);
    }
}

int Window::getMouseX() const { return this->mouseX; }

int Window::getMouseY() const { return this->mouseY; }

uint32_t Window::getMouseState() const { return this->mouseState; }

int Window::getWindowWidth() const { return this->windowWidth; }

int Window::getWindowHeight() const { return this->windowHeight; }

const uint8_t *Window::getKeyboard() const { return this->keyboard; }

const SDL_Window *Window::getFocus() const { return this->focus; }

const SDL_Window *Window::getWindow() const { return this->window; }

uint32_t Window::getWindowID() const { return this->windowID; }
