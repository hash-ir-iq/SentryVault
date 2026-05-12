#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <fstream>
#include <string>
#include "PasswordString.h"

// ---------------------------------------------------------
// Abstract Base Class
// ---------------------------------------------------------
class Vault_Record {
public:
    virtual std::string Get_Title() const = 0;
    virtual std::string Get_Username() const = 0;
    virtual std::string Get_URL() const = 0;

    virtual void displayInfo() const = 0;

    virtual void serialize(std::ostream& file) = 0;
    virtual void deserialize(std::istream& file) = 0;

    // OOP REQUIREMENT: Overloaded == for duplicate checking
    virtual bool operator==(const Vault_Record& other) const = 0;

    // OOP FLEX: Polymorphic Type ID for Factory Loading
    virtual int Get_Type() const = 0;

    virtual ~Vault_Record() {};

    // OOP REQUIREMENT: Overloaded << for formatted output
    friend std::ostream& operator<<(std::ostream& os, const Vault_Record& record) {
        record.displayInfo();
        return os;
    }
};

// ---------------------------------------------------------
// Child Class 1: PasswordEntry (Type 1)
// ---------------------------------------------------------
class PasswordEntry : public Vault_Record {
private:
    std::string   title;
    std::string   username;
    Secure_String password;
    std::string   URL;

public:
    PasswordEntry(const std::string& t, const std::string& u_name, const std::string& url, const Secure_String& p);

    std::string Get_Title() const override;
    std::string Get_Username() const override;
    std::string Get_URL() const override;
    Secure_String Get_Password() const;

    void displayInfo() const override;
    void serialize(std::ostream& file) override;
    void deserialize(std::istream& file) override;

    bool operator==(const Vault_Record& other) const override;

    // Returns 1 so LoadVault knows this is a Password
    int Get_Type() const override { return 1; }
};

// ---------------------------------------------------------
// Child Class 2: SecureNote (Type 2)
// ---------------------------------------------------------
class SecureNote : public Vault_Record {
private:
    std::string title;
    std::string content;

public:
    SecureNote(const std::string& t, const std::string& c);

    std::string Get_Title() const override;
    std::string Get_Username() const override;
    std::string Get_URL() const override;
    std::string Get_Content() const;

    void displayInfo() const override;
    void serialize(std::ostream& file) override;
    void deserialize(std::istream& file) override;

    bool operator==(const Vault_Record& other) const override;

    // Returns 2 so LoadVault knows this is a Note
    int Get_Type() const override { return 2; }
};

#endif // !DATA_H