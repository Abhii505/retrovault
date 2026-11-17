#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <algorithm>
#include <numeric>
#include <limits>
#include <fstream>

#include "Exceptions.h"
#include "GameROM.h"
#include "StorageDrive.h"
#include "PlaySessionLog.h"
#include "VaultUtils.h"

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void saveCatalogToFile(const std::map<std::string, GameROM*>& catalog) {
    std::ofstream out("vault_data.txt");
    if (!out) return;
    for (const auto& pair : catalog) {
        GameROM* r = pair.second;
        out << r->getTypeTag() << "," << r->getId() << "," << r->getTitle()
            << "," << r->getSizeMB() << "," << r->getReleaseYear()
            << "," << (r->isCorrupted() ? "CORRUPTED" : "OK") << "\n";
    }
    std::cout << "[File I/O] Saved " << catalog.size() << " ROM entries to vault_data.txt.\n";
}

int main() {
    std::map<std::string, GameROM*> romCatalog;
    std::vector<StorageDrive> drives;
    std::queue<std::string> launchQueue;
    std::list<PlaySessionLog> sessionHistory;

    drives.emplace_back("NVME0", "/mnt/roms_fast", 8192.0);
    PlaySessionLog activeSession("Rudra_P1");

    int choice = -1;
    do {
        std::cout << "\n=== RETROVAULT: CONSOLE GAME ROM MANAGER ===\n"
                  << "1. Add Console ROM\n"
                  << "2. Add Storage Drive\n"
                  << "3. Mount & Queue ROM for Launch\n"
                  << "4. Inspect / Filter Corrupted ROMs\n"
                  << "5. View Emulator Launch Queue\n"
                  << "6. Calculate ROM Storage Footprint\n"
                  << "7. Drive Audit & Debugger Reset\n"
                  << "8. Sort & View Play Session Logs\n"
                  << "9. Run OOP Diagnostics (Casts & Templates)\n"
                  << "0. Save & Exit\n"
                  << "Choice: ";

        if (!(std::cin >> choice)) {
            std::cout << "[Input Error] Enter a valid number.\n";
            clearInput();
            continue;
        }

        try {
            switch (choice) {
                case 1: {
                    int type, year, validFlag;
                    std::string id, title;
                    double sizeMB;
                    std::cout << "Console Type (1-GBA, 2-Nintendo DS, 3-Nintendo 3DS): ";
                    std::cin >> type;
                    std::cout << "ROM ID: "; std::cin >> id;
                    clearInput();
                    std::cout << "Game Title: "; std::getline(std::cin, title);
                    std::cout << "Size (MB): "; std::cin >> sizeMB;
                    std::cout << "Release Year: "; std::cin >> year;
                    std::cout << "Checksum Valid (1-Yes, 0-Corrupted): "; std::cin >> validFlag;

                    GameROM* newRom = nullptr;
                    if (type == 1) {
                        std::string saveType;
                        std::cout << "Save Type (e.g., SRAM, Flash1M, EEPROM): "; std::cin >> saveType;
                        newRom = new GBARom(id, title, sizeMB, year, validFlag == 1, saveType);
                    } else if (type == 2) {
                        bool touchMic;
                        std::cout << "Uses Touch/Mic (1/0): "; std::cin >> touchMic;
                        newRom = new DSRom(id, title, sizeMB, year, validFlag == 1, touchMic);
                    } else if (type == 3) {
                        bool encryptedCia;
                        std::cout << "Encrypted CIA (1/0): "; std::cin >> encryptedCia;
                        newRom = new ThreeDSRom(id, title, sizeMB, year, validFlag == 1, encryptedCia);
                    } else {
                        std::cout << "Unknown ROM type.\n";
                        break;
                    }

                    if (romCatalog.count(id)) delete romCatalog[id];
                    romCatalog[id] = newRom;
                    std::cout << "Added successfully: ";
                    newRom->display();
                    break;
                }
                case 2: {
                    std::string dId, path;
                    double cap;
                    std::cout << "Drive ID: "; std::cin >> dId;
                    std::cout << "Mount Path: "; std::cin >> path;
                    std::cout << "Capacity (MB): "; std::cin >> cap;
                    drives.emplace_back(dId, path, cap);
                    std::cout << "Drive mounted. Total active drives: "
                              << StorageDrive::getTotalDrives() << "\n";
                    break;
                }
                case 3: {
                    std::string id;
                    std::cout << "Enter ROM ID to mount & launch: ";
                    std::cin >> id;
                    if (!romCatalog.count(id)) {
                        throw InvalidROMException("ROM ID '" + id + "' not found in catalog.");
                    }
                    GameROM* rom = romCatalog[id];
                    if (rom->isCorrupted()) {
                        throw CorruptedChecksumException("Refusing to launch '" + rom->getTitle() + "' (Bad CRC32).");
                    }
                    drives[0].allocateROM(rom->getSizeMB());
                    drives[0].logDailyLaunches(0, 1);
                    activeSession += rom->getSizeMB();
                    launchQueue.push(rom->getTitle() + " [" + rom->emulatorCore() + "]");
                    std::cout << "Mounted on " << drives[0].getId() << " & queued for launch.\n";
                    break;
                }
                case 4: {
                    int corruptCount = std::count_if(romCatalog.begin(), romCatalog.end(),
                        [](const std::pair<const std::string, GameROM*>& entry) {
                            return entry.second->isCorrupted();
                        });
                    std::cout << "Total ROMs: " << romCatalog.size()
                              << " | Corrupted Dumps: " << corruptCount << "\n";
                    for (auto it = romCatalog.begin(); it != romCatalog.end(); ++it) {
                        it->second->display();
                    }
                    break;
                }
                case 5: {
                    if (launchQueue.empty()) {
                        std::cout << "Launch queue is empty.\n";
                        break;
                    }
                    std::queue<std::string> temp = launchQueue;
                    int pos = 1;
                    while (!temp.empty()) {
                        std::cout << pos++ << ". " << temp.front() << "\n";
                        temp.pop();
                    }
                    break;
                }
                case 6: {
                    std::string id;
                    int copies;
                    std::cout << "Enter ROM ID: "; std::cin >> id;
                    if (!romCatalog.count(id)) throw InvalidROMException("ROM not found.");
                    std::cout << "Number of backup copies: "; std::cin >> copies;
                    double totalMB = calculateStorageFootprint(*romCatalog[id], copies);
                    int wholeMB = static_cast<int>(totalMB); // static_cast requirement
                    std::cout << "Exact Footprint (with " << romCatalog[id]->compressionOverhead()
                              << "% overhead): " << totalMB << " MB (Whole MB: " << wholeMB << ")\n";
                    break;
                }
                case 7: {
                    generateVaultAudit(drives[0]);
                    EmulatorDebugger dbg;
                    dbg.inspectAndResetAlerts(drives[0]);
                    break;
                }
                case 8: {
                    if (activeSession.getCount() > 0) {
                        sessionHistory.push_back(std::move(activeSession)); // std::move requirement
                        activeSession = PlaySessionLog("Rudra_P2");
                    }
                    sessionHistory.sort(); // Uses operator<
                    for (const auto& s : sessionHistory) std::cout << s << "\n";
                    break;
                }
                case 9: {
                    // 1. dynamic_cast demo - safely downcast to ThreeDSRom
                    for (const auto& pair : romCatalog) {
                        ThreeDSRom* threed = dynamic_cast<ThreeDSRom*>(pair.second);
                        if (threed != nullptr) {
                            std::cout << "[dynamic_cast] 3DS ROM '" << threed->getTitle()
                                      << "' AES Keys required: " << (threed->needsAesKeys() ? "YES" : "NO") << "\n";
                        }
                    }
                    // 2. const_cast legacy string example
                    const std::string header = "AGB_GBA_CART_HDR_V1";
                    printLegacyCartridgeLabel(const_cast<char*>(header.c_str()));

                    // 3. Template & const char* specialization demo
                    std::cout << "[Template maxOf<const char*>] Later alphabetically: "
                              << maxOf("Pokemon Emerald", "Zelda Phantom Hourglass") << "\n";

                    VaultRegistry<std::string> emuRegistry;
                    emuRegistry.add("mGBA");
                    emuRegistry.add("melonDS");
                    const std::string* found = emuRegistry.findIf([](const std::string& s) {
                        return s == "melonDS";
                    });
                    if (found) std::cout << "[VaultRegistry::findIf] Located emulator: " << *found << "\n";
                    break;
                }
                case 0:
                    saveCatalogToFile(romCatalog);
                    break;
                default:
                    std::cout << "Invalid choice (0-9).\n";
            }
        } catch (const VaultException& e) {
            std::cerr << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Standard Exception] " << e.what() << "\n";
        } catch (...) {
            std::cerr << "[Unknown Error] Unexpected failure caught.\n";
        }
    } while (choice != 0);

    // Clean up heap-allocated GameROM pointers
    for (auto& pair : romCatalog) {
        delete pair.second;
    }
    return 0;
}