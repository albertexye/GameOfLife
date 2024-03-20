#pragma once

#include <SDL2/SDL.h>

#include <cinttypes>
#include <thread>
#include <mutex>
#include <chrono>
#include <barrier>

#include "../component.h"
#include "CoreExtension.h"
#include "CoreRenderer.h"
#include "CoreExtension.h"
#include "CoreSelector.h"

class CoreThread {
public:
    std::thread thread;
    int index{};
    Core &core;

    CoreThread(int index, Core &core);

    [[noreturn]] void run();
};

class Core : public Component {
private:
    std::vector<uint8_t> map{}, buffer{};
    int mapWidth{}, mapHeight{};
    std::mutex bufferLock{};

    int threadNum{};
    std::vector<CoreThread> threads{};
    std::unique_ptr<std::thread> mainThread{};
    std::barrier<> threadStartBarrier;
    std::barrier<> threadEndBarrier;

    bool pauseFlag{true};
    bool stopFlag{false};
    bool ready{false};

    double speed{0.};
    double gps{0.};

    friend class CoreThread;

    inline void setBuffer(int x, int y, bool value) {
        const auto index = y * this->mapWidth + x;
        if (value) this->buffer[index / 8] |= 1 << (index % 8);
        else this->buffer[index / 8] &= ~(1 << (index % 8));
    }

    inline void swap() {
        std::unique_lock<std::mutex> lock(this->bufferLock);
        this->map.swap(this->buffer);
        std::fill(this->buffer.begin(), this->buffer.end(), 0);
    }

    [[nodiscard]] inline bool rangeCheck(int x, int y) const {
        return x < 0 || y < 0 || x >= this->mapWidth || y >= this->mapHeight;
    }

    [[nodiscard]] inline static uint64_t getTime() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    void run();

    void stop();

    void simulate(int yStart, int yEnd);

    bool handle(SDL_Event &event) override;

    void save(std::ofstream &stream) const override;

    void load(std::ifstream &stream) override;

public:
    Core(int width, int height, double speed, int threadNum);

    ~Core();

    [[nodiscard]] inline bool read(int x, int y) const {
        const auto index = y * this->mapWidth + x;
        return ((this->map[index / 8] & (1 << (index % 8))) != 0);
    }

    inline void set(int x, int y, bool value) {
        if (this->rangeCheck(x, y)) return;
        const auto index = y * this->mapWidth + x;
        if (value) this->map[index / 8] |= 1 << (index % 8);
        else this->map[index / 8] &= ~(1 << (index % 8));
    }

    void step();

    void lockBuffer();

    void unlockBuffer();

    [[nodiscard]] bool getPause() const;

    void setPause(bool pause);

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    void setSize(int width, int height);

    [[nodiscard]] double getSpeed() const;

    [[nodiscard]] double getActualSpeed() const;

    void setSpeed(double newSpeed);

    [[nodiscard]] int getThreadNum() const;

    void setThreadNum(int newThreadNum);

    [[nodiscard]] bool getReady() const;
};
