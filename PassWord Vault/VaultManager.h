#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H
#include <string>
#include "DynamicArray_Holder.h"
#include "Vault_Record.h"
#include "PasswordEntry.h"
#include "SecureNote.h"

class VaultManager {
private:
    DynamicArray<Vault_Record*> Entries_array;
    bool        is_Unlocked;

    std::string current_username;

    std::string Hashed_Master_Password;
    std::string Temp_Master_Key;
    int         failed_attempts;

    void encrypt_Decrypt(char* data, int size, const std::string& key);

public:
    VaultManager();
    ~VaultManager();

    bool Create_Vault(const std::string& username, const std::string& master_pass);
    bool Login(const std::string& username, const std::string& input_pass);
    void Logout();

    void Add_Entry(Vault_Record* ptr);
    void Delete_Entry(int index);

    DynamicArray<Vault_Record*>& GetArray();

    void SaveVault();
    void LoadVault();
};

#endif // !VAULTMANAGER_H