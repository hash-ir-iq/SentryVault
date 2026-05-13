#include "Exceptions.h"

VaultException::VaultException(const std::string& mes) : message(mes) {}
const char* VaultException::what() const noexcept { 
	return message.c_str();
}
VaultException::~VaultException() {}

AuthException::AuthException(const std::string& mes, int attempt) : VaultException(mes), attempts(attempt) {}
int AuthException::Get_Attempts() const {
	return attempts;
}

Bad_Alloc::Bad_Alloc(const std::string& errorText, int sizeToAllocate) : VaultException(errorText), Failed_Size_Allocation(sizeToAllocate) {}
int Bad_Alloc::getFailedSize() const {
	return Failed_Size_Allocation;
}

DuplicateEntryException::DuplicateEntryException(const std::string& mes) : VaultException(mes) {}
ValidationException::ValidationException(const std::string& mes) : VaultException(mes) {}