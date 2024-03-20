#include "CoreExtension.h"

#include "../clip/clip.h"
#include "Core.h"

CoreExtension::CoreExtension(Core &core) : core(core) {}

void CoreExtension::copy(int x, int y, int width, int height) const {
    if (x >= this->core.getWidth()) return;
    if (y >= this->core.getHeight()) return;
    if (x + width >= this->core.getWidth()) width = this->core.getWidth() - 1 - x;
    if (y + height >= this->core.getHeight()) height = this->core.getHeight() - 1 - y;

    std::vector<uint8_t> value(width * height + sizeof(width) + sizeof(height));

    memcpy(value.data(), &width, sizeof(width));
    memcpy(value.data() + sizeof(width), &height, sizeof(height));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            value[i * width + j + sizeof(width) + sizeof(height)] = this->core.read(j + x, i + y);
        }
    }

    clip::format gameOfLifeFormat = clip::register_format("com.gameoflife.gol");
    clip::lock lock;
    lock.clear();
    lock.set_data(gameOfLifeFormat, reinterpret_cast<const char *>(value.data()), value.size());
}

void CoreExtension::paste(int x, int y) {
    if (x >= this->core.getWidth()) return;
    if (y >= this->core.getHeight()) return;

    int width, height;

    clip::format gameOfLifeFormat = clip::register_format("com.gameoflife.gol");
    clip::lock lock;
    if (!lock.is_convertible(gameOfLifeFormat)) return;
    std::vector<uint8_t> value(lock.get_data_length(gameOfLifeFormat));
    lock.get_data(gameOfLifeFormat, reinterpret_cast<char *>(value.data()), value.size());
    memcpy(&width, value.data(), sizeof(width));
    memcpy(&height, value.data() + sizeof(width), sizeof(height));

    if (x + width >= this->core.getWidth()) width = this->core.getWidth() - 1 - x;
    if (y + height >= this->core.getHeight()) height = this->core.getHeight() - 1 - y;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            this->core.set(j + x, i + y, value[i * width + j + sizeof(width) + sizeof(height)]);
        }
    }
}

void CoreExtension::cut(int x, int y, int width, int height) {
    if (x >= this->core.getWidth()) return;
    if (y >= this->core.getHeight()) return;
    if (x + width >= this->core.getWidth()) width = this->core.getWidth() - 1 - x;
    if (y + height >= this->core.getHeight()) height = this->core.getHeight() - 1 - y;

    std::vector<uint8_t> value(width * height + sizeof(width) + sizeof(height));

    memcpy(value.data(), &width, sizeof(width));
    memcpy(value.data() + sizeof(width), &height, sizeof(height));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            value[i * width + j + sizeof(width) + sizeof(height)] = this->core.read(j + x, i + y);
            this->core.set(j + x, i + y, false);
        }
    }

    clip::format gameOfLifeFormat = clip::register_format("com.gameoflife.gol");
    clip::lock lock;
    lock.clear();
    lock.set_data(gameOfLifeFormat, reinterpret_cast<const char *>(value.data()), value.size());
}

void CoreExtension::fill(int x, int y, int width, int height) {
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            this->core.set(j, i, true);
        }
    }
}

void CoreExtension::empty(int x, int y, int width, int height) {
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            this->core.set(j, i, false);
        }
    }
}

void CoreExtension::inverse(int x, int y, int width, int height) {
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            this->core.set(j, i, !this->core.read(j, i));
        }
    }
}
