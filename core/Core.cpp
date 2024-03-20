#include "Core.h"

#include <thread>
#include <fstream>
#include <vector>

#include "../clip/clip.h"

CoreThread::CoreThread(int index, Core &core) : index{index}, core{core}, thread{&CoreThread::run, this} {}

[[noreturn]] void CoreThread::run() {
    const auto chunkSize = this->core.mapHeight / this->core.threadNum;
    const auto yStart = chunkSize * this->index;
    auto yEnd = yStart + chunkSize;
    if (yEnd > this->core.mapHeight) yEnd = this->core.mapHeight;

    while (true) {
        this->core.threadStartBarrier.arrive_and_wait();
        this->core.simulate(yStart, yEnd);
        this->core.threadEndBarrier.arrive_and_wait();
    }
}

void Core::run() {
    this->stopFlag = false;

    this->threads.reserve(this->threadNum - 1);
    for (int i = 1; i < this->threadNum; ++i) {
        this->threads.emplace_back(i, *this);
    }

    this->ready = true;

    uint64_t thisTime, lastTime;

    lastTime = Core::getTime();
    while (!this->stopFlag) {
        if (this->pauseFlag) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        this->step();

        thisTime = Core::getTime();
        auto actualTime = (double) (thisTime - lastTime);

        if (this->speed > 0.) {
            const double expectedTime = 1'000'000. / this->speed;
            if (this->speed > 60.) {
                while (actualTime < expectedTime) {
                    thisTime = Core::getTime();
                    actualTime = static_cast<double>(thisTime - lastTime);
                    std::this_thread::yield();
                }
            } else {
                while (actualTime < expectedTime && std::abs(actualTime - expectedTime) > 4000) {
                    thisTime = Core::getTime();
                    actualTime = static_cast<double>(thisTime - lastTime);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }

        this->gps = 1'000'000. / actualTime;
        lastTime = thisTime;
    }
    this->threads.clear();
}

void Core::stop() {
    this->ready = false;
    this->stopFlag = true;
    if (this->mainThread) this->mainThread->join();
}

void Core::simulate(int yStart, int yEnd) {
    bool l1, l2, l3, m1, m2, m3, r1, r2, r3;
    uint8_t count;
    int i = yStart, j;

    if (yStart == 0) {
        l2 = false;
        l3 = false;
        m2 = this->read(0, i);
        m3 = this->read(0, i + 1);

        for (j = 0; j < this->mapWidth - 1; ++j) {
            r2 = this->read(j + 1, i);
            r3 = this->read(j + 1, i + 1);

            count = l2 + l3 + m3 + r2 + r3;

            this->setBuffer(j, i, count == 3 || (m2 && count == 2));

            l2 = m2;
            l3 = m3;
            m2 = r2;
            m3 = r3;
        }
        count = l2 + l3 + m3;
        this->setBuffer(j, i, count == 3 || (m2 && count == 2));

        ++i;
    }

    for (; i < yEnd - (yEnd == this->mapHeight); ++i) {
        l1 = false;
        l2 = false;
        l3 = false;
        m1 = this->read(0, i - 1);
        m2 = this->read(0, i);
        m3 = this->read(0, i + 1);

        for (j = 0; j < this->mapWidth - 1; ++j) {
            r1 = this->read(j + 1, i - 1);
            r2 = this->read(j + 1, i);
            r3 = this->read(j + 1, i + 1);

            count = l1 + l2 + l3 + m1 + m3 + r1 + r2 + r3;

            this->setBuffer(j, i, count == 3 || (m2 && count == 2));

            l1 = m1;
            l2 = m2;
            l3 = m3;
            m1 = r1;
            m2 = r2;
            m3 = r3;
        }
        count = l1 + l2 + l3 + m1 + m3;
        this->setBuffer(j, i, count == 3 || (m2 && count == 2));
    }

    if (yEnd == this->mapHeight) {
        l1 = false;
        l2 = false;
        m1 = this->read(0, i - 1);
        m2 = this->read(0, i);

        for (j = 0; j < this->mapWidth - 1; ++j) {
            r1 = this->read(j + 1, i - 1);
            r2 = this->read(j + 1, i);

            count = l1 + l2 + m1 + r1 + r2;

            this->setBuffer(j, i, count == 3 || (m2 && count == 2));

            l1 = m1;
            l2 = m2;
            m1 = r1;
            m2 = r2;
        }
        count = l1 + l2 + m1;
        this->setBuffer(j, i, count == 3 || (m2 && count == 2));
    }
}

Core::Core(int width, int height, double speed, int threadNum) : Component{0, 0, 0, 0}, threadStartBarrier{threadNum}, threadEndBarrier{threadNum} {
    if (width % 8 != 0) {
        width += 8 - width % 8;
    }
    if (height % 8 != 0) {
        height += 8 - height % 8;
    }

    this->mapWidth = width;
    this->mapHeight = height;
    this->speed = speed;

    this->map.resize(width * height / 8, 0);
    this->buffer.resize(width * height / 8, 0);

    if (threadNum < 1) threadNum = 1;
    this->threadNum = threadNum;
    this->mainThread = std::make_unique<std::thread>(&Core::run, this);
}

Core::~Core() { this->stop(); }

void Core::step() {
    this->threadStartBarrier.arrive_and_wait();
    this->simulate(0, this->mapHeight / this->threadNum);
    this->threadEndBarrier.arrive_and_wait();
    this->swap();
}

void Core::lockBuffer() { this->bufferLock.lock(); }

void Core::unlockBuffer() { this->bufferLock.unlock(); }

void Core::setPause(bool pause) { this->pauseFlag = pause; }

[[nodiscard]] bool Core::getPause() const { return this->pauseFlag; }

[[nodiscard]] double Core::getActualSpeed() const {
    if (this->pauseFlag || this->stopFlag) return 0.;
    return this->gps;
}

void Core::save(std::ofstream &stream) const {
    uint32_t width = this->mapWidth;
    uint32_t height = this->mapHeight;
    stream.write(reinterpret_cast<const char *>(&width), sizeof(width));
    stream.write(reinterpret_cast<const char *>(&height), sizeof(height));
    stream.write(reinterpret_cast<const char *>(&this->speed), sizeof(this->speed));
    stream.write(reinterpret_cast<const char *>(this->map.data()), static_cast<long long>(this->map.size()));
}

void Core::load(std::ifstream &stream) {
    uint32_t width, height;
    stream.read(reinterpret_cast<char *>(&width), sizeof(width));
    stream.read(reinterpret_cast<char *>(&height), sizeof(height));
    stream.read(reinterpret_cast<char *>(&this->speed), sizeof(this->speed));

    this->mapWidth = static_cast<int>(width);
    this->mapHeight = static_cast<int>(height);

    this->map.resize(this->mapWidth * this->mapHeight / 8);
    this->buffer.resize(this->mapWidth * this->mapHeight / 8, 0);
    stream.read(reinterpret_cast<char *>(this->map.data()), static_cast<long long>(this->map.size()));

    this->pauseFlag = true;
}

bool Core::handle(SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case ' ':
                case 'p':
                    this->pauseFlag = !this->pauseFlag;
                    return true;
                case 'n':
                    if (this->getPause()) this->step();
                    return true;
            }
            break;
    }
    return false;
}

[[nodiscard]] int Core::getWidth() const { return this->mapWidth; }

[[nodiscard]] int Core::getHeight() const { return this->mapHeight; }

void Core::setSize(int width, int height) {
    if (width == this->mapWidth && height == this->mapHeight) return;

    this->stop();

    this->buffer.resize(width * height / 8, 0);

    int maxWidth = this->mapWidth;
    if (maxWidth > width) maxWidth = width;

    int maxHeight = this->mapHeight;
    if (maxHeight > height) maxHeight = height;

    std::vector<uint8_t> newMap(width * height / 8, 0);
    for (int i = 0; i < maxHeight; ++i) {
        memcpy(newMap.data() + i * width / 8, this->map.data() + i * this->mapWidth / 8, maxWidth / 8);
    }

    this->map.swap(newMap);

    this->mapWidth = width;
    this->mapHeight = height;

    this->run();
}

double Core::getSpeed() const { return this->speed; }

void Core::setSpeed(double newSpeed) { this->speed = newSpeed; }

int Core::getThreadNum() const { return this->threadNum; }

void Core::setThreadNum(int newThreadNum) {
    if (newThreadNum == this->threadNum) return;

    this->stop();

    this->threadNum = newThreadNum;

    this->run();
}

bool Core::getReady() const {
    return this->ready;
}
