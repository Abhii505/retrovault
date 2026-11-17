#include "StorageDrive.h"
#include "Exceptions.h"

int StorageDrive::totalDrives = 0;

StorageDrive::StorageDrive()
    : driveId("DRV0"), mountPath("/mnt/default"), totalCapacityMB(4096.0),
      usedSpaceMB(0.0), corruptAlertCount(0) {
    for (int i = 0; i < 7; ++i) dailyLaunchCount[i] = 0.0;
    ++totalDrives;
}

StorageDrive::StorageDrive(const std::string& id, const std::string& path, double capacityMB)
    : driveId(id), mountPath(path), totalCapacityMB(capacityMB),
      usedSpaceMB(0.0), corruptAlertCount(0) {
    for (int i = 0; i < 7; ++i) dailyLaunchCount[i] = 0.0;
    ++totalDrives;
}

StorageDrive::StorageDrive(const StorageDrive& other)
    : driveId(other.driveId + "_COPY"), mountPath(other.mountPath),
      totalCapacityMB(other.totalCapacityMB), usedSpaceMB(other.usedSpaceMB),
      corruptAlertCount(other.corruptAlertCount) {
    for (int i = 0; i < 7; ++i) dailyLaunchCount[i] = other.dailyLaunchCount[i];
    ++totalDrives;
}

StorageDrive::StorageDrive(StorageDrive&& other) noexcept
    : driveId(std::move(other.driveId)), mountPath(std::move(other.mountPath)),
      totalCapacityMB(other.totalCapacityMB), usedSpaceMB(other.usedSpaceMB),
      corruptAlertCount(other.corruptAlertCount) {
    for (int i = 0; i < 7; ++i) {
        dailyLaunchCount[i] = other.dailyLaunchCount[i];
        other.dailyLaunchCount[i] = 0.0;
    }
    other.totalCapacityMB = 0.0;
    other.usedSpaceMB = 0.0;
    ++totalDrives;
}

StorageDrive::~StorageDrive() {
    --totalDrives;
    std::cout << "[Log] Unmounting StorageDrive '" << driveId
              << "'. Active drives remaining: " << totalDrives << "\n";
}

void StorageDrive::allocateROM(double romSizeMB) {
    if (usedSpaceMB + romSizeMB > totalCapacityMB) {
        ++corruptAlertCount;
        throw InsufficientStorageException(
            "Drive " + driveId + " needs " + std::to_string(romSizeMB) +
            " MB, only " + std::to_string(getFreeSpaceMB()) + " MB free.");
    }
    usedSpaceMB += romSizeMB;
}

void StorageDrive::logDailyLaunches(int dayIndex, double launches) {
    if (dayIndex >= 0 && dayIndex < 7 && launches >= 0) {
        dailyLaunchCount[dayIndex] = launches;
    }
}

double StorageDrive::totalWeeklyLaunches() const {
    double sum = 0.0;
    for (int i = 0; i < 7; ++i) sum += dailyLaunchCount[i];
    return sum;
}

void generateVaultAudit(const StorageDrive& drive) {
    std::cout << "=== VAULT STORAGE AUDIT [" << drive.driveId << "] ===\n"
              << "Mount Path    : " << drive.mountPath << "\n"
              << "Used / Total  : " << drive.usedSpaceMB << " / " << drive.totalCapacityMB << " MB\n"
              << "Alert Counter : " << drive.corruptAlertCount << "\n"
              << "Weekly Plays  : " << drive.totalWeeklyLaunches() << "\n";
}

void EmulatorDebugger::inspectAndResetAlerts(StorageDrive& drive) {
    std::cout << "[EmulatorDebugger] Drive " << drive.driveId
              << " had " << drive.corruptAlertCount << " alerts. Resetting to 0.\n";
    drive.corruptAlertCount = 0;
}