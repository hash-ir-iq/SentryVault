#include "Data.h"

    
PasswordEntry::PasswordEntry(const std::string& t, const std::string& u_name, const std::string& url, const Secure_String& p)
    : title(t), username(u_name), URL(url), password(p) {
}

std::string PasswordEntry::Get_Title() const { return title; }
std::string PasswordEntry::Get_Username() const { return username; }
std::string PasswordEntry::Get_URL() const { return URL; }
Secure_String PasswordEntry::Get_Password() const { return password; }

void PasswordEntry::displayInfo() const {
    std::cout << "Title: " << title << "\n";
    std::cout << "Username: " << username << "\n";
    std::cout << "URL: " << URL << "\n";
}

// ------------------------------------------------------------------
// THIS FIXES YOUR ERROR: Operator == Implementation
// ------------------------------------------------------------------
bool PasswordEntry::operator==(const Vault_Record& other) const {
    // Strict Title match. If the Title already exists, block it.
    return (this->title == other.Get_Title());
}


void PasswordEntry::serialize(std::ostream& file) {
    int len;

    len = (int)title.length();
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


// ==================================================================
// CHILD CLASS 2: SecureNote Implementation
// ==================================================================

// Constructor
SecureNote::SecureNote(const std::string& t, const std::string& c)
    : title(t), content(c) {
}

// Getters
std::string SecureNote::Get_Title() const { return title; }

// A note doesn't have a username or URL, so we return empty strings
std::string SecureNote::Get_Username() const { return ""; }
std::string SecureNote::Get_URL() const { return ""; }

std::string SecureNote::Get_Content() const { return content; }

// Display
void SecureNote::displayInfo() const {
    std::cout << "--- SECURE NOTE ---\n";
    std::cout << "Title: " << title << "\n";
    std::cout << "Content: " << content << "\n";
    std::cout << "-------------------\n";
}

// Operator == for Duplicate Checking
bool SecureNote::operator==(const Vault_Record& other) const {
    // Blocks notes from having the exact same title as another note or password
    return (this->title == other.Get_Title());
}

// Binary Serialization
void SecureNote::serialize(std::ostream& file) {
    int len;

    // Write Title
    len = (int)title.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(title.c_str(), len);

    // Write Content
    len = (int)content.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(content.c_str(), len);
}

// Binary Deserialization
void SecureNote::deserialize(std::istream& file) {
    int len;

    // Read Title
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    title.resize(len);
    file.read(&title[0], len);

    // Read Content
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    content.resize(len);
    file.read(&content[0], len);
}