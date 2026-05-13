#ifndef VAULT_RECORD_H
#define VAULT_RECORD_H

#include <iostream>
#include <fstream>
#include <string>

class Vault_Record {
public:
    virtual std::string Get_Title() const = 0;
    virtual std::string Get_Username() const = 0;
    virtual std::string Get_URL() const = 0;

    virtual void displayInfo() const = 0;

    virtual void serialize(std::ostream& file) = 0;
    virtual void deserialize(std::istream& file) = 0;

    virtual bool operator==(const Vault_Record& other) const = 0;
    virtual int Get_Type() const = 0;

    virtual ~Vault_Record() {};

    friend std::ostream& operator<<(std::ostream& os, const Vault_Record& record) {
        record.displayInfo();
        return os;
    }
};

#endif // !VAULT_RECORD_H