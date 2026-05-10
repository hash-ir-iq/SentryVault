# Secure Password Vault

A local, offline password manager built from scratch in C++. 

This project was developed as a 2nd-semester systems programming and OOP assignment for FAST NUCES. The primary constraint was to build the entire backend architecture **without using the C++ Standard Template Library (STL)**. No `<vector>`, no `<string>`, and no built-in memory management tools were allowed.

## Features
- **Master Password Authentication:** Locks the vault and generates the key for encryption.
- **Offline Storage:** Passwords are saved locally; no cloud or external APIs are used.
- **View & Delete:** Easily add, view, and remove saved credentials from the dashboard.
- **Graphical Interface:** Built using [Raylib](https://www.raylib.com/) and `raygui` for a simple, memory-safe UI.

## Under the Hood (Technical Details)
Since STL was banned for this project, the core data structures and security measures were built manually:
- **Custom `DynamicArray`:** A templated, auto-resizing array that handles heap allocations. Deletions use an $O(1)$ swap-and-shrink method to avoid $O(N)$ array shifting.
- **Custom `Secure_String`:** A manual character buffer implementation that automatically wipes plaintext passwords from RAM (`memset` to zero) before freeing the heap memory to prevent memory scraping.
- **Binary Serialization & Cryptography:** Data is not saved as plain text. It is serialized into a raw binary format (`std::fstream` in binary mode) and encrypted at rest using a modulo-based XOR cipher tied to the Master Password.
- **Polymorphism:** The GUI interacts with a `Vault_Record` base class via pure virtual functions, eliminating the need for brittle `dynamic_cast` checks.

## Build Instructions (Visual Studio 2022)

Because the project relies on Raylib and standard C file I/O operations, you need to configure a few settings in Visual Studio to compile it successfully.

1. **Install Raylib:** - Open your project in VS 2022.
   - Go to `Project` -> `Manage NuGet Packages`.
   - Search for `raylib` (by v_2samg) and click Install.
2. **Add Raygui:**
   - Download the single `raygui.h` header file from the official raygui repository.
   - Drop it directly into your project's header files folder.
3. **Disable MSVC Security Warnings:**
   - MSVC will block `raygui.h` from compiling due to standard C functions like `fopen`.
   - Right-click your **Project** (not the Solution) -> `Properties`.
   - Go to `C/C++` -> `Preprocessor`.
   - Add `_CRT_SECURE_NO_WARNINGS` to the Preprocessor Definitions.
4. **Compile and Run** (F5).

## Usage Notes
- **First-time run:** The app will ask you to create a Master Password. This creates the initial `vault.dat` file.
- **Don't forget your Master Password.** Since encryption is tied directly to the password hash, there is no "Forgot Password" recovery option. If you lose it, the `vault.dat` file remains permanently locked as binary gibberish.

---
*Disclaimer: This is an academic project built for educational purposes regarding manual memory management and basic cryptography.*
