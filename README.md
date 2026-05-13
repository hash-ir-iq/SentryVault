# Secure Password Vault (Multi-Tenant)

A locally encrypted, multi-user password and secure note manager built entirely in C++. 

This project was engineered to demonstrate strict systems-level C++ architecture. It intentionally bypasses the Standard Template Library (STL) to implement manual heap memory management, custom template data structures, and polymorphic binary serialization. The graphical user interface is powered by Raylib.

## Core Architectural Features

* **Manual Memory Management:** Bypasses `std::vector` in favor of a custom-built `DynamicArray<T>` template class. Handles dynamic resizing and strict heap scrubbing to prevent memory leaks and data bleeding.
* **Polymorphic Data Models:** Utilizes an Abstract Base Class (`Vault_Record`) to enforce a strict contract for `PasswordEntry` and `SecureNote` child classes, allowing both types to exist within a single dynamic array.
* **Multi-Tenant Routing:** Dynamically generates and routes isolated binary files (`[username]_vault.dat`) for different users, preventing a single point of failure.
* **Security & RAM Scrubbing:** Implements XOR encryption for file storage and features an aggressive memory scrub that manually deletes heap pointers upon user logout to secure local RAM.
* **Custom Exception Pipeline:** Replaces standard error codes with a custom exception hierarchy (`AuthException`, `Bad_Alloc`, `DuplicateEntryException`) to gracefully trap and handle runtime failures.
* **Operator Overloading:** Implements custom `==` and `<<` operators for strict duplicate entry prevention and stream formatting.

## File Structure

The architecture strictly adheres to separation of concerns (declarations in `.h`, definitions in `.cpp`), resulting in 13 atomic files:

**Application Entry**
* `main.cpp` - Raylib UI state machine and event loop.

**Core Engine**
* `VaultManager.h` / `VaultManager.cpp` - Central controller for multi-user routing, encryption, and file I/O.
* `DynamicArray_Holder.h` - Custom template array for dynamic heap allocation.

**Data Models**
* `Vault_Record.h` - Abstract Base Class interface.
* `PasswordEntry.h` / `PasswordEntry.cpp` - Implementation for credential storage.
* `SecureNote.h` / `SecureNote.cpp` - Implementation for multi-line text storage.
* `PasswordString.h` / `PasswordString.cpp` - Low-level char buffer management for sensitive strings.

**Exception Handling**
* `Exceptions.h` / `Exceptions.cpp` - Custom error routing hierarchy.

## Compilation & Build Instructions

This project requires a C++ compiler (MSVC, GCC, or Clang) and the **Raylib** library for UI rendering.

### Dependencies
* [Raylib](https://www.raylib.com/) (Graphics framework)
* `raygui.h` (Immediate-mode GUI header for Raylib)

### Building with MSVC (Visual Studio)
1. Open your Visual Studio Solution.
2. Ensure all `.cpp` files are added to the **Source Files** folder.
3. Ensure all `.h` files are added to the **Header Files** folder.
4. Verify Raylib is properly linked in your project properties (Include Directories and Library Directories).
5. Build and Run (F5).

## Security Warning
This vault utilizes a basic XOR cipher for local file obfuscation. It is designed as an architectural demonstration of C++ systems programming, not as a production-ready replacement for enterprise password managers with AES-256 encryption.
