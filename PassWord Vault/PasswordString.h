#ifndef PASSWORDSTRING
#define PASSWORDSTRING

class Secure_String {
private:
    char* buffer;
    int Password_Size;
public:
    Secure_String();
    Secure_String(const char* temp_buffer, int s);
    Secure_String(const Secure_String& obj);

    Secure_String& operator=(const Secure_String& obj);

    void Deleting_Buffer(char* b, int size);
    void Copy_Data(const char* Source, int Source_Size, char* Target);

    int Size_Getter();
    const char* Get_SecureString_Ptr() const;

    ~Secure_String();
};

#endif // !PASSWORDSTRING