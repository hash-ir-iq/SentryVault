#include "SecureNote.h"

SecureNote::SecureNote(const std::string& t, const std::string& c) : title(t), content(c) {}

std::string SecureNote::Get_Title() const { return title; }
std::string SecureNote::Get_Username() const { return ""; }
std::string SecureNote::Get_URL() const { return ""; }
std::string SecureNote::Get_Content() const { return content; }

void SecureNote::displayInfo() const {
    std::cout << "--- SECURE NOTE ---\n";
    std::cout << "Title: " << title << "\n";
    std::cout << "Content: " << content << "\n";
    std::cout << "-------------------\n";
}

bool SecureNote::operator==(const Vault_Record& other) const {
    return (this->title == other.Get_Title());
}

int SecureNote::Get_Type() const { return 2; }

void SecureNote::serialize(std::ostream& file) {
    int len = (int)title.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(title.c_str(), len);

    len = (int)content.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    file.write(content.c_str(), len);
}

void SecureNote::deserialize(std::istream& file) {
    int len;
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    title.resize(len);
    file.read(&title[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    content.resize(len);
    file.read(&content[0], len);
}