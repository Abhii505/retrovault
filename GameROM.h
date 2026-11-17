#ifndef GAMEROM_H
#define GAMEROM_H

#include <string>
#include <iostream>

class GameROM {
protected:
    std::string romId;
    std::string title;
    double sizeMB;
    int releaseYear;
    bool validChecksum; // false = corrupted dump (analogous to expired medicine)

public:
    GameROM(const std::string& id, const std::string& title, double sizeMB,
            int releaseYear, bool validChecksum);
    virtual ~GameROM(); // Virtual destructor

    // Pure virtual functions
    virtual std::string emulatorCore() const = 0;
    virtual double compressionOverhead() const = 0;
    virtual std::string getTypeTag() const = 0;

    // Virtual display
    virtual void display() const;

    // Getters
    std::string getId() const { return romId; }
    std::string getTitle() const { return title; }
    double getSizeMB() const { return sizeMB; }
    int getReleaseYear() const { return releaseYear; }
    bool isCorrupted() const { return !validChecksum; }
};

class GBARom : public GameROM {
private:
    std::string saveType; // e.g., "SRAM", "Flash1M", "EEPROM"
public:
    GBARom(const std::string& id, const std::string& title, double sizeMB,
           int year, bool valid, const std::string& saveType);
    std::string emulatorCore() const override;
    double compressionOverhead() const override;
    std::string getTypeTag() const override { return "GBA"; }
    void display() const override;
};

class DSRom : public GameROM {
private:
    bool usesTouchMic;
public:
    DSRom(const std::string& id, const std::string& title, double sizeMB,
          int year, bool valid, bool usesTouchMic);
    std::string emulatorCore() const override;
    double compressionOverhead() const override;
    std::string getTypeTag() const override { return "NDS"; }
    bool hasTouchMicFeatures() const { return usesTouchMic; }
    void display() const override;
};

class ThreeDSRom : public GameROM {
private:
    bool isEncryptedCia;
public:
    ThreeDSRom(const std::string& id, const std::string& title, double sizeMB,
               int year, bool valid, bool isEncryptedCia);
    std::string emulatorCore() const override;
    double compressionOverhead() const override;
    std::string getTypeTag() const override { return "3DS"; }
    bool needsAesKeys() const { return isEncryptedCia; }
    void display() const override;
};

#endif