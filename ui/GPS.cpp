#include "GPS.h"

#include "Font.h"

void GPS::render(SDL_Renderer *renderer) {
    char buffer[32];
    const auto gps = this->core.getActualSpeed();
    sprintf(buffer, "GPS:%.6g", gps);
    this->font.render(renderer, buffer, 8, 8);
}

GPS::GPS(const Core &core) : Component{0, 0, 0, 0}, core{core} {
    this->font.setSize(2);
    this->font.setColor(0, 0, 255, 64);
    this->font.setSpace(2);
}
