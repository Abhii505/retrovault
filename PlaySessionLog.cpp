#include "PlaySessionLog.h"
#include <stdexcept>
#include <utility>

PlaySessionLog::PlaySessionLog(const std::string& tag)
    : playerTag(tag), count(0), capacity(2) {
    sessionSizesMB = new double[capacity];
}

PlaySessionLog::~PlaySessionLog() {
    delete[] sessionSizesMB;
}

PlaySessionLog::PlaySessionLog(const PlaySessionLog& other)
    : playerTag(other.playerTag), count(other.count), capacity(other.capacity) {
    sessionSizesMB = new double[capacity];
    for (int i = 0; i < count; ++i) {
        sessionSizesMB[i] = other.sessionSizesMB[i];
    }
}

PlaySessionLog& PlaySessionLog::operator=(const PlaySessionLog& other) {
    if (this != &other) {
        delete[] sessionSizesMB;
        playerTag = other.playerTag;
        count = other.count;
        capacity = other.capacity;
        sessionSizesMB = new double[capacity];
        for (int i = 0; i < count; ++i) {
            sessionSizesMB[i] = other.sessionSizesMB[i];
        }
    }
    return *this;
}

PlaySessionLog::PlaySessionLog(PlaySessionLog&& other) noexcept
    : playerTag(std::move(other.playerTag)), sessionSizesMB(other.sessionSizesMB),
      count(other.count), capacity(other.capacity) {
    other.sessionSizesMB = nullptr;
    other.count = 0;
    other.capacity = 0;
}

PlaySessionLog& PlaySessionLog::operator=(PlaySessionLog&& other) noexcept {
    if (this != &other) {
        delete[] sessionSizesMB;
        playerTag = std::move(other.playerTag);
        sessionSizesMB = other.sessionSizesMB;
        count = other.count;
        capacity = other.capacity;
        other.sessionSizesMB = nullptr;
        other.count = 0;
        other.capacity = 0;
    }
    return *this;
}

void PlaySessionLog::resize() {
    capacity *= 2;
    double* newArr = new double[capacity];
    for (int i = 0; i < count; ++i) newArr[i] = sessionSizesMB[i];
    delete[] sessionSizesMB;
    sessionSizesMB = newArr;
}

PlaySessionLog& PlaySessionLog::operator+=(double romSizeMB) {
    if (count == capacity) resize();
    sessionSizesMB[count++] = romSizeMB;
    return *this;
}

double PlaySessionLog::operator[](int index) const {
    if (index < 0 || index >= count) throw std::out_of_range("Session index out of bounds.");
    return sessionSizesMB[index];
}

double PlaySessionLog::totalSizeMB() const {
    double total = 0.0;
    for (int i = 0; i < count; ++i) total += sessionSizesMB[i];
    return total;
}

bool PlaySessionLog::operator==(const PlaySessionLog& other) const {
    return totalSizeMB() == other.totalSizeMB();
}

bool PlaySessionLog::operator<(const PlaySessionLog& other) const {
    return totalSizeMB() < other.totalSizeMB();
}

std::ostream& operator<<(std::ostream& os, const PlaySessionLog& log) {
    os << "Session [" << log.playerTag << "] | ROMs Loaded: " << log.count
       << " | Total Footprint: " << log.totalSizeMB() << " MB";
    return os;
}