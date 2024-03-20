#pragma once

#include "../Component.h"
#include "../RectBuffer.h"

class Core;

class CoreRenderer;

class CoreExtension;

class CoreSelector : public Component {
private:
    int startMapX = 0, startMapY = 0, endMapX = 0, endMapY = 0;
    int lastMapX = -1, lastMapY = -1;
    bool selecting = false, updating = false;

    Core &core;
    CoreRenderer &coreRenderer;
    CoreExtension &coreExtension;

    RectBuffer rectLiveBuffer{32};
    RectBuffer rectDeadBuffer{32};

    bool handle(SDL_Event &event) override;

    void render(SDL_Renderer *renderer) override;

    void save(std::ofstream &stream) const override;

    void load(std::ifstream &stream) override;

    void swap(int &xStart, int &xEnd, int &yStart, int &yEnd) const;

public:
    explicit CoreSelector(Core &core, CoreRenderer &coreRenderer, CoreExtension &coreExtension);

    void startSelect();

    void updateSelect();

    void endSelect();

    void clearSelect();

    void toggle();

    void clearToggle();

    void copy() const;

    void paste();

    void cut();

    void fill();

    void empty();

    void inverse();
};
