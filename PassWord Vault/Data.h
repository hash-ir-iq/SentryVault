#ifndef DATA
#define DATA

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "PasswordString.h"

class Vault_Record {
public:
    virtual std::string Get_Title() = 0;
    virtual std::string Get_Username() = 0;
    virtual std::string Get_URL() = 0;

    virtual void displayInfo() const = 0;

    // std::ostream / std::istream so ANY stream works:
    // fstream, ofstream, ostringstream, istringstream - all compatible.
    virtual void serialize(std::ostream& file) = 0;
    virtual void deserialize(std::istream& file) = 0;

    virtual ~Vault_Record() {};
};


class PasswordEntry : public Vault_Record {
private:
    std::string   title;
    std::string   username;
    Secure_String password;
    std::string   URL;

public:
    PasswordEntry(const std::string& t, const std::string& u_name,
        const std::string& url, const Secure_String& p)
        : title(t), username(u_name), URL(url), password(p) {
    }

    std::string Get_Title()    override { return title; }
    std::string Get_Username() override { return username; }
    std::string Get_URL()      override { return URL; }

    Secure_String Get_Password() { return password; }

    void displayInfo() const override {
        std::cout << "Title: " << title << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "URL: " << URL << std::endl;
    }

    void serialize(std::ostream& file) override {
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

    void deserialize(std::istream& file) override {
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
};

#endif // !DATA