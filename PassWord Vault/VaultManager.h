#ifndef VAULTMANAGER
#define VAULTMANAGER

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstring>

#include "DynamicArray_Holder.h"
#include "Data.h"
#include "Exceptions.h"

class VaultManager {
private:
    DynamicArray<Vault_Record*> Entries_array;
    bool        is_Unlocked;
    std::string Hashed_Master_Password;
    std::string Temp_Master_Key;
    int         failed_attempts = 0;

    void encrypt_Decrypt(char* data, int size, const std::string& key) {
        int keyLen = (int)key.length();
        if (keyLen == 0) return;
        for (int i = 0; i < size; i++)
            data[i] ^= key[i % keyLen];
    }

public:
    VaultManager() : is_Unlocked(false) {}

    bool Create_Vault(const std::string& master_pass) {
        Hashed_Master_Password = std::to_string(std::hash<std::string>{}(master_pass));
        Temp_Master_Key = master_pass;
        is_Unlocked = true;
        failed_attempts = 0;
        return true;
    }

    void LoadHash() {
        std::ifstream file("vault.dat", std::ios::binary);
        if (!file.is_open()) return;

        int hash_len = 0;
        file.read(reinterpret_cast<char*>(&hash_len), sizeof(int));

        if (hash_len <= 0 || hash_len > 512) return;

        Hashed_Master_Password.resize(hash_len);
        file.read(&Hashed_Master_Password[0], hash_len);
    }

    bool Login(const std::string& input_pass) {
        std::string input_hash = std::to_string(std::hash<std::string>{}(input_pass));

        if (input_hash == Hashed_Master_Password) {
            is_Unlocked = true;
            Temp_Master_Key = input_pass;
            failed_attempts = 0;
            return true;
        }
        failed_attempts++;
        throw AuthException("CRITICAL: Incorrect Master Password!", failed_attempts);
    }

    void Logout() {
        // SYSTEMS FIX: We MUST save the data to the hard drive before we lock the vault 
        // and wipe the key from RAM. If we don't, all new entries are lost.
        SaveVault();
        is_Unlocked = false;
        Temp_Master_Key = "";
    }

    void Add_Entry(Vault_Record* ptr) {
        if (is_Unlocked)
            Entries_array.AddEntry(ptr);
    }

    void Delete_Entry(int index) {
        if (!is_Unlocked) return;
        if (index >= 0 && index < Entries_array.Get_Size()) {
            delete Entries_array[index];
            Entries_array.RemoveEntry(index);
        }
    }

    DynamicArray<Vault_Record*>& GetArray() { return Entries_array; }

    void SaveVault() {
        if (!is_Unlocked || Temp_Master_Key.empty()) return;

        // 1. Serialize everything to a raw binary temp file to avoid stringstream corruption
        std::ofstream tempOut("temp.dat", std::ios::binary | std::ios::trunc);
        int total = Entries_array.Get_Size();
        tempOut.write(reinterpret_cast<char*>(&total), sizeof(int));
        for (int i = 0; i < total; i++) {
            Entries_array[i]->serialize(tempOut);
        }
        tempOut.close();

        // 2. Read the binary temp file into a raw char array
        std::ifstream tempIn("temp.dat", std::ios::binary | std::ios::ate);
        int dataSize = (int)tempIn.tellg();
        tempIn.seekg(0);

        char* buf = new char[dataSize];
        tempIn.read(buf, dataSize);
        tempIn.close();

        // 3. Encrypt the raw bytes
        encrypt_Decrypt(buf, dataSize, Temp_Master_Key);

        // 4. Write final vault.dat
        std::ofstream out("vault.dat", std::ios::binary | std::ios::trunc);
        int hash_len = (int)Hashed_Master_Password.length();
        out.write(reinterpret_cast<char*>(&hash_len), sizeof(int));
        out.write(Hashed_Master_Password.c_str(), hash_len);
        out.write(buf, dataSize);
        out.close();

        // Wipe RAM and delete temp file
        memset(buf, 0, dataSize);
        delete[] buf;
        remove("temp.dat");
    }

    void LoadVault() {
        std::ifstream file("vault.dat", std::ios::binary | std::ios::ate);
        if (!file.is_open()) return;

        int totalSize = (int)file.tellg();
        file.seekg(0);

        int hash_len = 0;
        file.read(reinterpret_cast<char*>(&hash_len), sizeof(int));
        file.seekg(sizeof(int) + hash_len);

        int dataSize = totalSize - (int)sizeof(int) - hash_len;
        if (dataSize <= 0) return;

        char* buf = new char[dataSize];
        file.read(buf, dataSize);
        file.close();

        // Decrypt in place
        encrypt_Decrypt(buf, dataSize, Temp_Master_Key);

        // Write decrypted bytes to temp.dat so we can safely deserialize
        std::ofstream tempOut("temp.dat", std::ios::binary | std::ios::trunc);
        tempOut.write(buf, dataSize);
        tempOut.close();

        memset(buf, 0, dataSize);
        delete[] buf;

        // Rebuild objects from temp.dat
        std::ifstream tempIn("temp.dat", std::ios::binary);
        int total = 0;
        tempIn.read(reinterpret_cast<char*>(&total), sizeof(int));

        for (int i = 0; i < total; i++) {
            Secure_String empty;
            PasswordEntry* entry = new PasswordEntry("", "", "", empty);
            entry->deserialize(tempIn);
            Entries_array.AddEntry(entry);
        }
        tempIn.close();
        remove("temp.dat");
    }

    ~VaultManager() {
        for (int i = 0; i < Entries_array.Get_Size(); i++)
            delete Entries_array[i];
    }
};

#endif // !VAULTMANAGER