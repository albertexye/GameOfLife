#pragma once

#include "../Component.h"
#include "../RectBuffer.h"

class Core;

class CoreRenderer : public Component {
private:
    Core &core;

    double startMapX = 0, startMapY = 0;
    int blockSize{};

    RectBuffer rectBuffer{64};

    void update() override;

    bool handle(SDL_Event &event) override;

    void render(SDL_Renderer *renderer) override;

    void save(std::ofstream &stream) const override;

    void load(std::ifstream &stream) override;

    void calibrate();

public:
    CoreRenderer(Core &core, int blockSize);

    void move(double unitX, double unitY);

    void zoom(int unitN);

    [[nodiscard]] double getStartMapX() const;

    [[nodiscard]] double getStartMapY() const;

    [[nodiscard]] int getBlockSize() const;
};
