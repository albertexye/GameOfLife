#pragma once

#include <cinttypes>

class Core;

class CoreExtension {
private:
    Core &core;

public:
    explicit CoreExtension(Core &core);

    void copy(int x, int y, int width, int height) const;

    void paste(int x, int y);

    void cut(int x, int y, int width, int height);

    void fill(int x, int y, int width, int height);

    void empty(int x, int y, int width, int height);

    void inverse(int x, int y, int width, int height);
};
