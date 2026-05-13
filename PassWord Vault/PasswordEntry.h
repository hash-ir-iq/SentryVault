
#ifndef PASSWORDENTRY_H
#define PASSWORDENTRY_H

#include "Vault_Record.h"
#include "PasswordString.h"
#include <string>

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
    int Get_Type() const override;
};

#endif // !PASSWORDENTRY_H