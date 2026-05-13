#ifndef SECURENOTE_H
#define SECURENOTE_H

#include "Vault_Record.h"
#include <string>

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
    int Get_Type() const override;
};

#endif // !SECURENOTE_H