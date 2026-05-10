#ifndef DATA
#define DATA

#include<iostream>
#include<fstream>
#include "PasswordString.h"

class Vault_Record {
public:

	virtual void displayInfo() const = 0;
	virtual void serialize(std::fstream& file) = 0;
	virtual ~Vault_Record() {};
};


class PasswordEntry : public Vault_Record {
private: 
	std::string title;
	std::string username;
	Secure_String password;
	std::string URL;

public:
	PasswordEntry(const std::string& t, const std::string& u_name, const std::string& url,const Secure_String& p) 
		:	title(t),username(u_name),URL(url),password(p)
	{};

	void displayInfo() const override {
		std::cout << "Title: " << title <<std::endl;
		std::cout << "Username: " << username << std::endl;
		std::cout << "URL : " << URL << std::endl;

	}

    void serialize(std::fstream& file) override {
        int len;

        // 1. Serialize Title
        len = title.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(title.c_str(), len);

        // 2. Serialize Username
        len = username.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(username.c_str(), len);

        // 3. Serialize URL
        len = URL.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(URL.c_str(), len);

        // 4. Serialize Secure_String (Password)
        len = password.Size_Getter();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(password.Get_SecureString_Ptr(), len);
    }




};








#endif // !DATA

