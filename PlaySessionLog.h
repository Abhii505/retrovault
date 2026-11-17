#ifndef PLAYSESSIONLOG_H
#define PLAYSESSIONLOG_H

#include <iostream>
#include <string>

class PlaySessionLog {
private:
    std::string playerTag;
    double* sessionSizesMB; // Dynamic array of ROM sizes launched in session
    int count;
    int capacity;

    void resize();

public:
    explicit PlaySessionLog(const std::string& tag = "Player1");

    // Rule of Five
    ~PlaySessionLog();
    PlaySessionLog(const PlaySessionLog& other);
    PlaySessionLog& operator=(const PlaySessionLog& other);
    PlaySessionLog(PlaySessionLog&& other) noexcept;
    PlaySessionLog& operator=(PlaySessionLog&& other) noexcept;

    double totalSizeMB() const;
    int getCount() const { return count; }
    std::string getPlayerTag() const { return playerTag; }

    // Overloaded operators
    PlaySessionLog& operator+=(double romSizeMB);
    double operator[](int index) const;
    bool operator==(const PlaySessionLog& other) const;
    bool operator<(const PlaySessionLog& other) const;
    friend std::ostream& operator<<(std::ostream& os, const PlaySessionLog& log);
};

#endif