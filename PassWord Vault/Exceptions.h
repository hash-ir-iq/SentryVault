#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
#include <iostream>

class VaultException {
protected:
    std::string message;
public:
    VaultException(const std::string& mes);
    virtual const char* what() const noexcept;
    virtual ~VaultException();
};

class AuthException : public VaultException {
private:
    int attempts;
public:
    AuthException(const std::string& mes, int attempt);
    int Get_Attempts() const;
};

class Bad_Alloc : public VaultException {
private:
    int Failed_Size_Allocation;
public:
    Bad_Alloc(const std::string& errorText, int sizeToAllocate);
    int getFailedSize() const;
};

class DuplicateEntryException : public VaultException {
public:
    DuplicateEntryException(const std::string& mes);
};

class ValidationException : public VaultException {
public:
    ValidationException(const std::string& mes);
};

#endif // !EXCEPTIONS_H