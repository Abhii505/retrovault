#ifndef STORAGEDRIVE_H
#define STORAGEDRIVE_H

#include <string>
#include <iostream>

class EmulatorDebugger; // Forward declaration for friend class

class StorageDrive {
private:
    std::string driveId;
    std::string mountPath;
    double totalCapacityMB;
    double usedSpaceMB;
    int corruptAlertCount;
    double dailyLaunchCount[7]; // 7-day ROM launch activity array

    static int totalDrives;
    static constexpr double LOW_SPACE_THRESHOLD_MB = 512.0;

public:
    StorageDrive();
    StorageDrive(const std::string& id, const std::string& path, double capacityMB);
    StorageDrive(const StorageDrive& other);
    StorageDrive(StorageDrive&& other) noexcept;
    ~StorageDrive();

    void allocateROM(double romSizeMB);
    void logDailyLaunches(int dayIndex, double launches);
    double totalWeeklyLaunches() const;

    // Const getters
    std::string getId() const { return driveId; }
    std::string getPath() const { return mountPath; }
    double getFreeSpaceMB() const { return totalCapacityMB - usedSpaceMB; }
    bool isLowSpace() const { return getFreeSpaceMB() < LOW_SPACE_THRESHOLD_MB; }

    static int getTotalDrives() { return totalDrives; }

    // Friend function and friend class
    friend void generateVaultAudit(const StorageDrive& drive);
    friend class EmulatorDebugger;
};

class EmulatorDebugger {
public:
    void inspectAndResetAlerts(StorageDrive& drive);
};

#endif