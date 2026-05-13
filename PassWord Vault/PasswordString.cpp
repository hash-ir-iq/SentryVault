#include "PasswordString.h"
#include <cstring>

Secure_String::Secure_String() : Password_Size(0) { buffer = nullptr; }

Secure_String::Secure_String(const char* temp_buffer, int s) {
    this->Password_Size = s;
    buffer = new char[Password_Size + 1];
    Copy_Data(temp_buffer, Password_Size, buffer);
}

Secure_String::Secure_String(const Secure_String& obj) {
    this->Password_Size = obj.Password_Size;
    buffer = new char[this->Password_Size + 1];
    Copy_Data(obj.buffer, obj.Password_Size, buffer);
}

Secure_String& Secure_String::operator=(const Secure_String& obj) {
    if (this == &obj) return *this;
    Deleting_Buffer(buffer, Password_Size);
    this->Password_Size = obj.Password_Size;
    buffer = new char[Password_Size + 1];
    Copy_Data(obj.buffer, obj.Password_Size, buffer);
    return *this;
}

void Secure_String::Deleting_Buffer(char* b, int size) {
    if (b == nullptr) return;
    memset(b, 0, size + 1);
    delete[] b;
}

void Secure_String::Copy_Data(const char* Source, int Source_Size, char* Target) {
    for (int i = 0; i < Source_Size; i++) Target[i] = Source[i];
    Target[Source_Size] = '\0';
}

int Secure_String::Size_Getter() { return Password_Size; }

const char* Secure_String::Get_SecureString_Ptr() const { return buffer ? buffer : ""; }

Secure_String::~Secure_String() { Deleting_Buffer(buffer, Password_Size); }