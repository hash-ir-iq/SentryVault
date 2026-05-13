#include "PasswordEntry.h"
#include <cstring>

PasswordEntry::PasswordEntry(const std::string& t, const std::string& u_name, const std::string& url, const Secure_String& p)
    : title(t), username(u_name), URL(url), password(p) {
}

std::string PasswordEntry::Get_Title() const { return title; }
std::string PasswordEntry::Get_Username() const { return username; }
std::string PasswordEntry::Get_URL() const { return URL; }
Secure_String PasswordEntry::Get_Password() const { return password; }

void PasswordEntry::displayInfo() const {
    std::cout << "Title: " << title << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "URL: " << URL << std::endl;
}

bool PasswordEntry::operator==(const Vault_Record& other) const {
    return (this->title == other.Get_Title());
}

int PasswordEntry::Get_Type() const { return 1; }

void PasswordEntry::serialize(std::ostream& file) {
    int len = (int)title.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(title.c_str(), len);

    len = (int)username.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(username.c_str(), len);

    len = (int)URL.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(URL.c_str(), len);

    len = password.Size_Getter();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(password.Get_SecureString_Ptr(), len);
}

void PasswordEntry::deserialize(std::istream& file) {
    int len;
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    title.resize(len);
    file.read(&title[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    username.resize(len);
    file.read(&username[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    URL.resize(len);
    file.read(&URL[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    char* temp_buffer = new char[len + 1];
    file.read(temp_buffer, len);
    temp_buffer[len] = '\0';
    password = Secure_String(temp_buffer, len);
    memset(temp_buffer, 0, len + 1);
    delete[] temp_buffer;
}