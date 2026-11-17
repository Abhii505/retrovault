#include "GameROM.h"
#include "Exceptions.h"
#include <iomanip>

GameROM::GameROM(const std::string& id, const std::string& title, double sizeMB,
                 int releaseYear, bool validChecksum)
    : romId(id), title(title), sizeMB(sizeMB),
      releaseYear(releaseYear), validChecksum(validChecksum) {
    if (romId.empty() || title.empty()) {
        throw InvalidROMException("ROM ID and Title cannot be empty.");
    }
    if (sizeMB <= 0.0) {
        throw InvalidROMException("ROM file size must be greater than 0 MB.");
    }
    if (releaseYear < 2001 || releaseYear > 2026) {
        throw InvalidROMException("Release year must be between 2001 and 2026.");
    }
}

GameROM::~GameROM() {
    // Virtual destructor ensures derived resources clean up properly
}

void GameROM::display() const {
    std::cout << "[" << getTypeTag() << "] " << romId << " | " << title
              << " (" << releaseYear << ") | " << std::fixed << std::setprecision(2)
              << sizeMB << " MB | CRC: " << (validChecksum ? "OK" : "CORRUPTED");
}

// --- GBARom ---
GBARom::GBARom(const std::string& id, const std::string& title, double sizeMB,
               int year, bool valid, const std::string& saveType)
    : GameROM(id, title, sizeMB, year, valid), saveType(saveType) {}

std::string GBARom::emulatorCore() const {
    return "mGBA / VBA-M (Save: " + saveType + ")";
}

double GBARom::compressionOverhead() const {
    return 2.5; // 2.5% .gba to .zip header overhead
}

void GBARom::display() const {
    GameROM::display();
    std::cout << " | Save Type: " << saveType << " | Core: " << emulatorCore() << "\n";
}

// --- DSRom ---
DSRom::DSRom(const std::string& id, const std::string& title, double sizeMB,
             int year, bool valid, bool usesTouchMic)
    : GameROM(id, title, sizeMB, year, valid), usesTouchMic(usesTouchMic) {}

std::string DSRom::emulatorCore() const {
    return "melonDS / DeSmuME";
}

double DSRom::compressionOverhead() const {
    return 6.0; // 6.0% .nds trimmed/archive overhead
}

void DSRom::display() const {
    GameROM::display();
    std::cout << " | Touch/Mic: " << (usesTouchMic ? "Yes" : "No")
              << " | Core: " << emulatorCore() << "\n";
}

// --- ThreeDSRom ---
ThreeDSRom::ThreeDSRom(const std::string& id, const std::string& title, double sizeMB,
                       int year, bool valid, bool isEncryptedCia)
    : GameROM(id, title, sizeMB, year, valid), isEncryptedCia(isEncryptedCia) {}

std::string ThreeDSRom::emulatorCore() const {
    return std::string("Azahar / Citra (AES Keys: ") + (isEncryptedCia ? "Required" : "Decrypted") + ")";
}

double ThreeDSRom::compressionOverhead() const {
    return 12.0; // 12.0% .3ds/.cia NCCH partition & shader cache overhead
}

void ThreeDSRom::display() const {
    GameROM::display();
    std::cout << " | Encrypted CIA: " << (isEncryptedCia ? "Yes" : "No")
              << " | Core: " << emulatorCore() << "\n";
}