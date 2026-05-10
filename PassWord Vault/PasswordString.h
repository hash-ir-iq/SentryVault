#ifndef PASSWORDSTRING
#define PASSWORDSTRING

#include<cstring>

class Secure_String {
private:
	char* buffer;
	int Password_Size;
public:

	//default constructor
	Secure_String() : Password_Size(0) {
		buffer = nullptr;
	}

	//parametertized
	Secure_String(const char* temp_buffer, int s) {
		this->Password_Size = s;
		buffer = new char[Password_Size+1];
		
		Copy_Data(temp_buffer, Password_Size, buffer);
	}
	
	//Copy constructor
	Secure_String(const Secure_String& obj) {	

		this->Password_Size = obj.Password_Size;
		buffer = new char[this->Password_Size+1];
		
		Copy_Data(obj.buffer, obj.Password_Size, buffer);
	}

	//Assingment operater
	Secure_String& operator=(const Secure_String& obj) {
		
		//checking self assingment
		if (this == &obj) 
			return *this;
		
		//Deleting the already present password in it, no need to copying 
		Deleting_Buffer(buffer, Password_Size);
		
		this->Password_Size = obj.Password_Size;
		buffer = new char[Password_Size+1];
		Copy_Data(obj.buffer, obj.Password_Size, buffer);
		
		return *this;
	}

	void Deleting_Buffer(char * b, int size) {

		if (b == nullptr)
			return;

		//this overwrites the whole password buffer to 0 before deleting it, so the password is not 
		//also present in the RAM, 
		memset(b, 0, size+1);
		delete[]  b;
	}

	void Copy_Data(const char* Source, int Source_Size, char* Target) {
		
		for (int i = 0; i < Source_Size; i++)
			Target[i] = Source[i];

		Target[Source_Size] = '\0';
	}

	int Size_Getter() {
		return Password_Size;
	}

	const char* Get_SecureString_Ptr() const {
		
		//checks if the buffer is nullptr, sends "" arguement
		//using ternary for variety
		return buffer ? buffer : "";	
	}

	~Secure_String() {
		Deleting_Buffer(buffer, Password_Size);
	}
};

#endif // !PASSWORDSTRING