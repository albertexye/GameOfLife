#pragma once

#include <SDL2/SDL.h>

#include <fstream>

class Window;

class Component {
private:
    const Window *window{};

    double x, y;
    double width, height;

    void setWindow(Window *window);

    friend class Window;

protected:
    Component(double x, double y, double width, double height);

    [[nodiscard]] const Window *getWindow() const;

    void setSize(double x, double y, double width, double height);

    [[nodiscard]] double getComponentX() const;

    [[nodiscard]] double getComponentY() const;

    [[nodiscard]] double getComponentWidth() const;

    [[nodiscard]] double getComponentHeight() const;

    [[nodiscard]] int getComponentXP() const;

    [[nodiscard]] int getComponentYP() const;

    [[nodiscard]] int getComponentWidthP() const;

    [[nodiscard]] int getComponentHeightP() const;

    virtual void update();

    virtual bool handle(SDL_Event &event);

    virtual void render(SDL_Renderer *renderer);

    virtual void save(std::ofstream &stream) const;

    virtual void load(std::ifstream &stream);
};
