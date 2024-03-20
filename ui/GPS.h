#pragma once

#include "../Component.h"
#include "../core/Core.h"
#include "Font.h"

class GPS : public Component {
private:
    const Core &core;
    void render(SDL_Renderer *renderer) override;
    Font font{};

public:
    GPS(const Core &core);
};
