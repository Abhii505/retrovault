#ifndef VAULTUTILS_H
#define VAULTUTILS_H

#include "GameROM.h"
#include <vector>
#include <cstring>
#include <cmath>
#include <iostream>

// Inline helpers
inline double applyOverhead(double baseMB, double overheadPercent) {
    return baseMB * (1.0 + overheadPercent / 100.0);
}

inline double roundTwoDecimals(double val) {
    return std::round(val * 100.0) / 100.0;
}

// 3 Overloaded Footprint Calculators
double calculateStorageFootprint(double rawSizeMB, int copies,
                                 double overheadPercent = 5.0, double dedupDiscountMB = 0.0);
double calculateStorageFootprint(const GameROM& rom, int copies);
double calculateStorageFootprint(const std::string& cloudSyncTier, double baseSizeMB);

// Legacy C-style function for const_cast demo
void printLegacyCartridgeLabel(char* rawLabel);

// Function Template + const char* specialization
template <typename T>
T maxOf(T a, T b) {
    return (a > b) ? a : b;
}

template <>
inline const char* maxOf<const char*>(const char* a, const char* b) {
    return (std::strcmp(a, b) > 0) ? a : b;
}

// Class Template VaultRegistry<T>
template <typename T>
class VaultRegistry {
private:
    std::vector<T> items;
public:
    void add(const T& item) { items.push_back(item); }
    size_t size() const { return items.size(); }

    void printAll() const {
        for (const auto& item : items) std::cout << item << "\n";
    }

    template <typename Predicate>
    const T* findIf(Predicate pred) const {
        for (const auto& item : items) {
            if (pred(item)) return &item;
        }
        return nullptr;
    }
};

#endif