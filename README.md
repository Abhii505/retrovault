# RetroVault

Menu-driven C++ program that manages a personal library of retro game ROMs — GBA, Nintendo DS, and 3DS dumps — across storage drives, with emulator launch queues and play session tracking.

Built as a 1-week mini project for **Object-Oriented Programming with C++**.

## Build

Needs a C++14 compiler. With MinGW/g++:

```
g++ -std=c++14 -Wall -Wextra -o RetroVault.exe main.cpp GameROM.cpp StorageDrive.cpp PlaySessionLog.cpp VaultUtils.cpp
```

Then run `./RetroVault.exe` (or `RetroVault.exe` on Windows).

## What it does

| # | Menu option | What happens |
|---|---|---|
| 1 | Add Console ROM | Pick GBA / DS / 3DS, enter ID, title, size, year, checksum. Validated on construction. |
| 2 | Add Storage Drive | Mount another drive with a capacity. |
| 3 | Mount & Queue ROM | Allocates space on the drive and pushes the title onto the launch queue. Refuses corrupted dumps. |
| 4 | Inspect Corrupted ROMs | Lists the catalog, flags bad CRC entries. |
| 5 | View Launch Queue | Prints the queue (copied out — queue isn't iterable). |
| 6 | Storage Footprint | Size + backup copies + compression overhead for a given ROM. |
| 7 | Drive Audit | Friend function prints private used-space/alert counters; `EmulatorDebugger` resets them. |
| 8 | Play Session Logs | Moves the active session into a sorted list. |
| 9 | OOP Diagnostics | Demos `dynamic_cast`, `const_cast`, template specialization, and a lambda `findIf`. |
| 0 | Save & Exit | Writes the catalog to `vault_data.txt` and frees everything. |

## Files

```
Exceptions.h        VaultException + 3 derived types
GameROM.h/.cpp      Abstract GameROM, GBARom, DSRom, ThreeDSRom
StorageDrive.h/.cpp Drive class, statics, friend function/class
PlaySessionLog.h/.cpp  Rule of Five, operator overloads, dynamic array
VaultUtils.h/.cpp   Footprint overloads, maxOf template, VaultRegistry<T>
main.cpp            Menu loop, file I/O, catalog management
```

## Concepts covered

Inheritance and pure virtual functions, constructors (default / parameterized / copy / move), static members, friend functions and friend classes, the Rule of Five with a `double*` member, function overloading with default arguments, exceptions (`try` / multi-`catch` / `catch(...)`), class and function templates with a `const char*` specialization, `static_cast` / `dynamic_cast` / `const_cast`, STL containers (`map`, `vector`, `queue`, `list`) and algorithms (`count_if`, `sort`, `find_if`) with lambdas, plus `ofstream` persistence.

## Notes

- Compression overhead numbers (2.5% / 6% / 12%) are made up for the exercise, not measured.
- Release years are validated between 2001–2026 in the `GameROM` constructor, so every derived type inherits the check.
- Every `new` has a matching `delete`; the catalog is cleared before exit, and `PlaySessionLog` handles its own buffer via RAII.
- Compile clean with `-Wall -Wextra` — no warnings.

## Sample session

```
Choice: 1   Console Type: 3   ROM ID: 3DS001
Game Title: Monster Hunter 4 Ultimate
Size (MB): 3200   Release Year: 2014   Checksum Valid: 1   Encrypted CIA (1/0): 1
Added successfully: [3DS] 3DS001 | Monster Hunter 4 Ultimate (2014) | 3200.00 MB |
CRC: OK | Encrypted CIA: Yes | Core: Azahar / Citra (AES Keys: Required)

Choice: 6   Enter ROM ID: 3DS001   Number of backup copies: 2
Exact Footprint (with 12.00% overhead): 7168.00 MB (Whole MB: 7168)

Choice: 0
[File I/O] Saved 3 ROM entries to vault_data.txt.
```

Full report (with source listing, UML diagram, and screenshots): `RetroVault_Project_Report_v2.docx`.
