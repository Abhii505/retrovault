#include "VaultUtils.h"

double calculateStorageFootprint(double rawSizeMB, int copies,
                                 double overheadPercent, double dedupDiscountMB) {
    double gross = applyOverhead(rawSizeMB * copies, overheadPercent) - dedupDiscountMB;
    return roundTwoDecimals(gross > 0 ? gross : 0.0);
}

double calculateStorageFootprint(const GameROM& rom, int copies) {
    // Polymorphic call to rom.compressionOverhead()
    double gross = applyOverhead(rom.getSizeMB() * copies, rom.compressionOverhead());
    return roundTwoDecimals(gross);
}

double calculateStorageFootprint(const std::string& cloudSyncTier, double baseSizeMB) {
    double multiplier = (cloudSyncTier == "PRO_RAID") ? 2.0 : 1.0; // Mirroring overhead
    return roundTwoDecimals(baseSizeMB * multiplier);
}

void printLegacyCartridgeLabel(char* rawLabel) {
    std::cout << "[LEGACY BIOS HEADER]: " << rawLabel << "\n";
}