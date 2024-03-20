#include "Component.h"

#include "Window.h"

Component::Component(double x, double y, double width, double height) : x{x}, y{y}, width{width}, height{height} {}

void Component::setWindow(Window *window) { this->window = window; }

void Component::setSize(double x, double y, double width, double height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

double Component::getComponentX() const { return this->x; }

double Component::getComponentY() const { return this->y; }

double Component::getComponentWidth() const { return this->width; }

double Component::getComponentHeight() const { return this->height; }

int Component::getComponentXP() const {
    return static_cast<int>(this->window->getWindowWidth() * this->x);
}

int Component::getComponentYP() const {
    return static_cast<int>(this->window->getWindowHeight() * this->y);
}

int Component::getComponentWidthP() const {
    return static_cast<int>(this->window->getWindowWidth() * this->width);
}

int Component::getComponentHeightP() const {
    return static_cast<int>(this->window->getWindowHeight() * this->height);
}

const Window *Component::getWindow() const { return this->window; }

void Component::update() {}

bool Component::handle(SDL_Event &event) { return false; }

void Component::render(SDL_Renderer *renderer) {}

void Component::save(std::ofstream &stream) const {}

void Component::load(std::ifstream &stream) {}
