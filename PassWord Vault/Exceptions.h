#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <cstring>
#include <iostream>
#include <string>

class VaultException {
protected:
    std::string message;
public:
    VaultException(const std::string& mes) : message(mes) {}
    virtual const char* what() const noexcept { 
        return message.c_str();
    }
    virtual ~VaultException() {}
};

class AuthException : public VaultException {
private:
    int attempts;
public:
    AuthException(const std::string& mes, int attempt) : VaultException(mes), attempts(attempt) {}
    int Get_Attempts() const {
        return attempts; 
    }
};

class Bad_Alloc : public VaultException {
private:
    int Failed_Size_Allocation;
public:
    Bad_Alloc(const std::string& errorText, int sizeToAllocate)
        : VaultException(errorText), Failed_Size_Allocation(sizeToAllocate) {
    }
    int getFailedSize() const {
        return Failed_Size_Allocation; 
    }
};

class DuplicateEntryException : public VaultException {
public:
    DuplicateEntryException(const std::string& mes) : VaultException(mes) {}
};

class ValidationException : public VaultException {
public:
    ValidationException(const std::string& mes) : VaultException(mes) {}
};

#endif // !EXCEPTIONS_H