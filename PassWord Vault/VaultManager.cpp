#include "VaultManager.h"
#include "Exceptions.h"
#include <fstream>
#include <cstring>
#include <cstdio>


VaultManager::VaultManager() : is_Unlocked(false), failed_attempts(0) {}


VaultManager::~VaultManager() {
    for (int i = 0; i < Entries_array.Get_Size(); i++) {
        delete Entries_array[i];
    }
}


void VaultManager::encrypt_Decrypt(char* data, int size, const std::string& key) {
    int keyLen = (int)key.length();
    if (keyLen == 0)
        return;
    for (int i = 0; i < size; i++)
        data[i] ^= key[i % keyLen];
}

bool VaultManager::Create_Vault(const std::string& master_pass) {
    Hashed_Master_Password = std::to_string(std::hash<std::string>{}(master_pass));
    Temp_Master_Key = master_pass;
    is_Unlocked = true;
    failed_attempts = 0;
    return true;
}

void VaultManager::LoadHash() {
    std::ifstream file("vault.dat", std::ios::binary);
    if (!file.is_open())
        return;

    int hash_len = 0;
    file.read(reinterpret_cast<char*>(&hash_len), sizeof(int));
    if (hash_len <= 0 || hash_len > 512)
        return;

    Hashed_Master_Password.resize(hash_len);
    file.read(&Hashed_Master_Password[0], hash_len);
}

bool VaultManager::Login(const std::string& input_pass) {
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

void VaultManager::Logout() {
    SaveVault();
    is_Unlocked = false;
    Temp_Master_Key = "";
}


void VaultManager::Add_Entry(Vault_Record* ptr) {
    if (!is_Unlocked) return;

    
    for (int i = 0; i < Entries_array.Get_Size(); i++) {

        if (*Entries_array[i] == *ptr) {
            delete ptr;
            throw DuplicateEntryException("Entry with this Title and Username already exists!");
        }
    }

    Entries_array.AddEntry(ptr);
}

void VaultManager::Delete_Entry(int index) {
    if (!is_Unlocked)
        return;
    if (index >= 0 && index < Entries_array.Get_Size()) {
        delete Entries_array[index];
        Entries_array.RemoveEntry(index);
    }
}

DynamicArray<Vault_Record*>& VaultManager::GetArray() {
    return Entries_array;
}

void VaultManager::SaveVault() {
    if (!is_Unlocked || Temp_Master_Key.empty()) return;

    std::ofstream tempOut("temp.dat", std::ios::binary | std::ios::trunc);
    int total = Entries_array.Get_Size();
    tempOut.write(reinterpret_cast<char*>(&total), sizeof(int));

    for (int i = 0; i < total; i++) {
        // OOP FLEX: Polymorphic Type ID Writing
        // Write the ID (1 for Password, 2 for Note) BEFORE writing the data
        int type = Entries_array[i]->Get_Type();
        tempOut.write(reinterpret_cast<char*>(&type), sizeof(int));

        // Now serialize the actual data
        Entries_array[i]->serialize(tempOut);
    }
    tempOut.close();

    std::ifstream tempIn("temp.dat", std::ios::binary | std::ios::ate);
    int dataSize = (int)tempIn.tellg();
    tempIn.seekg(0);

    char* buf = new char[dataSize];
    tempIn.read(buf, dataSize);
    tempIn.close();

    encrypt_Decrypt(buf, dataSize, Temp_Master_Key);

    std::ofstream out("vault.dat", std::ios::binary | std::ios::trunc);
    int hash_len = (int)Hashed_Master_Password.length();
    out.write(reinterpret_cast<char*>(&hash_len), sizeof(int));
    out.write(Hashed_Master_Password.c_str(), hash_len);
    out.write(buf, dataSize);
    out.close();

    memset(buf, 0, dataSize);
    delete[] buf;
    remove("temp.dat");
}

void VaultManager::LoadVault() {
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

    encrypt_Decrypt(buf, dataSize, Temp_Master_Key);

    std::ofstream tempOut("temp.dat", std::ios::binary | std::ios::trunc);
    tempOut.write(buf, dataSize);
    tempOut.close();

    memset(buf, 0, dataSize);
    delete[] buf;

    std::ifstream tempIn("temp.dat", std::ios::binary);
    int total = 0;
    tempIn.read(reinterpret_cast<char*>(&total), sizeof(int));

    for (int i = 0; i < total; i++) {
        // OOP FLEX: Polymorphic Factory Loading
        // Read the type ID first to know what object to construct
        int type = 0;
        tempIn.read(reinterpret_cast<char*>(&type), sizeof(int));

        if (type == 1) {
            Secure_String empty;
            PasswordEntry* p_entry = new PasswordEntry("", "", "", empty);
            p_entry->deserialize(tempIn);
            Entries_array.AddEntry(p_entry);
        }
        else if (type == 2) {
            SecureNote* s_note = new SecureNote("", "");
            s_note->deserialize(tempIn);
            Entries_array.AddEntry(s_note);
        }
    }
    tempIn.close();
    remove("temp.dat");
}