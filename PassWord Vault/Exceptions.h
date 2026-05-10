#ifndef EXCEPTIONS
#define EXCEPTIONS


#include<cstring>
#include<iostream>

class GeneralException
{
protected:
	std::string message;
	
public:
	
	GeneralException(const std::string& mes) :message(mes) {};

	virtual const char* what() const noexcept {
		return &message[0];
	}

	virtual ~GeneralException() {};

};


class AuthException : public GeneralException 
{
private:
	int attempts;
public:
	AuthException(const std::string& mes, int attempt) : GeneralException(mes) , attempts(attempt){};

	int Get_Attempts() {
		return attempts;
	}

};

class MemoryException : public GeneralException {
private:
	int Failed_Size_Allocation;
public:
	MemoryException(const std::string& errorText, int sizeToAllocate)
		: GeneralException(errorText), Failed_Size_Allocation(sizeToAllocate) { };

	int getFailedSize() { 
		return Failed_Size_Allocation; 
	}
};

#endif // !EXCEPTIONS
