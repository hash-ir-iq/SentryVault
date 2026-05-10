#ifndef VAULTMANAGER
#define VAULTMANAGER

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio> 

#include "DynamicArray_Holder.h"
#include "Data.h"
#include "Exceptions.h"


class VaultManager {
private:
    DynamicArray<Vault_Record*> Entries_array; 
    bool is_Unlocked;
    std::string Hashed_Master_Password;
    std::string Temp_Master_Key;
    int failed_attempts = 0;

    // XOR CIPHERING the password
    void encrypt_Decrypt(char* buffer_data, int Size, const std::string& key) {
        int keyLen = key.length();

        // division by zero
        if (keyLen == 0)
            return;

        // XOR each entry
        for (int i = 0; i < Size; i++) {
            buffer_data[i] = buffer_data[i] ^ key[i % keyLen];
        }
    }

public:
    // master user not logged in by default
    VaultManager() {
        is_Unlocked = false;
    }

    // creating initial vault and set password
    bool Create_Vault(const std::string& master_pass) {
        // hashing master password
        Hashed_Master_Password = std::to_string(std::hash<std::string>{}(master_pass));

        is_Unlocked = true;
        Temp_Master_Key = master_pass;
        failed_attempts = 0;
        return true;
    }

    bool Login(const std::string& input_pass) {
        // hashing the input to check if it matches
        std::string input_hash = std::to_string(std::hash<std::string>{}(input_pass));

        if (input_hash == Hashed_Master_Password) {
            is_Unlocked = true;
            Temp_Master_Key = input_pass; // saving key for XOR
            failed_attempts = 0; 
            return true;
        }
        else {
            failed_attempts++;
            throw AuthException("CRITICAL: Incorrect Master Password!", failed_attempts);
        }
    }

    
    void Logout() {
        is_Unlocked = false;
        Temp_Master_Key = ""; 
    }

    // adds entry to the array
    void Add_Entry(Vault_Record* ptr) {
        if (is_Unlocked == true) {
            Entries_array.AddEntry(ptr);
        }
    }

    void Delete_Entry(int index) {
        if (is_Unlocked == true) {
            int total_items = Entries_array.Get_Size();

            if (index >= 0 && index < total_items) {
                delete Entries_array[index];

                Entries_array.RemoveEntry(index);
            }
        }
    }

    DynamicArray<Vault_Record*>& GetArray() {
        return Entries_array;
    }

    // saveing data to file
    void SaveVault() {
        if (is_Unlocked == false)
            return;

        //temporary file 
        std::fstream tempFile("temp.dat", std::ios::out | std::ios::binary);

        int TotalItems = Entries_array.Get_Size();
        //total size at top
        tempFile.write(reinterpret_cast<char*>(&TotalItems), sizeof(int));

        //serializing 
        for (int i = 0; i < TotalItems; i++) {
            Entries_array[i]->serialize(tempFile);
        }
        tempFile.close();

        // encryption
        std::fstream readTemp("temp.dat", std::ios::in | std::ios::binary | std::ios::ate);
        int FileSize = readTemp.tellg();
        readTemp.seekg(0, std::ios::beg);

        char* FileBuffer = new char[FileSize];
        readTemp.read(FileBuffer, FileSize);
        readTemp.close();

        // encrypting the buffer containing whole file
        encrypt_Decrypt(FileBuffer, FileSize, Temp_Master_Key);

        // writing encrypted buffer actual vault file
        std::fstream finalFile("vault.dat", std::ios::out | std::ios::binary);
        finalFile.write(FileBuffer, FileSize);
        finalFile.close();

        delete[] FileBuffer;
        remove("temp.dat");
    }

    void LoadVault() {
        std::fstream file("vault.dat", std::ios::in | std::ios::binary | std::ios::ate);
        
        // file doesnt exist yet
        if (!file.is_open())
            return; 

        int FileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        //transfering whole encrypted file into buffer
        char* buffer = new char[FileSize];
        file.read(buffer, FileSize);
        file.close();

        //decryption 
        encrypt_Decrypt(buffer, FileSize, Temp_Master_Key);

        // writing decrypted data to temp file
        std::fstream tempFile("temp.dat", std::ios::out | std::ios::binary);
        tempFile.write(buffer, FileSize);
        tempFile.close();

        memset(buffer, 0, FileSize);
        delete[] buffer;

        std::fstream readTemp("temp.dat", std::ios::in | std::ios::binary);

        int TotalItems;
        
        readTemp.read(reinterpret_cast<char*>(&TotalItems), sizeof(int));

        for (int i = 0; i < TotalItems; i++) {
            Secure_String empty_str;
            PasswordEntry* new_entry = new PasswordEntry("", "", "", empty_str);
            new_entry->deserialize(readTemp);

            Entries_array.AddEntry(new_entry);
        }

        readTemp.close();
        remove("temp.dat"); 
    }

    ~VaultManager() {
        for (int i = 0; i < Entries_array.Get_Size(); i++)  
            delete Entries_array[i];
    }
};

#endif // !VAULTMANAGER