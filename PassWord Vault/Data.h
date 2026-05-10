#ifndef DATA
#define DATA

#include<iostream>
#include<fstream>
#include<string>
#include "PasswordString.h"

class Vault_Record {
public:

    virtual std::string Get_Title() = 0;
    virtual std::string Get_Username() = 0;
    virtual std::string Get_URL() = 0;

	virtual void displayInfo() const = 0;
	virtual void serialize(std::fstream& file) = 0;
    virtual void deserialize(std::fstream& file) = 0;
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

    //GEtter functions
    std::string Get_Title() override{ 
        return title; 
    }
    std::string Get_Username() override {
        return username; 
    }
    std::string Get_URL() override {
        return URL; 
    }
    Secure_String Get_Password()  {
        return password; 
    }

	void displayInfo() const override {
		std::cout << "Title: " << title <<std::endl;
		std::cout << "Username: " << username << std::endl;
		std::cout << "URL : " << URL << std::endl;

	}

    void serialize(std::fstream& file) override {
        int len;

        //serializing Title
        len = title.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(title.c_str(), len);

        //serialize username
        len = username.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(username.c_str(), len);

        //serialize URL
        len = URL.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(URL.c_str(), len);

        //serializing Password
        len = password.Size_Getter();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        file.write(password.Get_SecureString_Ptr(), len);
    }

    void deserialize(std::fstream& file) override {
        int len;

        //deserialize Title
        //getting len of title
        file.read(reinterpret_cast<char*>(&len), sizeof(int));
        //resizing the title according to the lenght
        title.resize(len);
        //reading and storing it
        file.read(&title[0], len);

        //deserialize username
        file.read(reinterpret_cast<char*>(&len), sizeof(int));
        username.resize(len);
        file.read(&username[0], len);

        //deserialize URL
        file.read(reinterpret_cast<char*>(&len), sizeof(int));
        URL.resize(len);
        file.read(&URL[0], len);

        //reading password lenght
        file.read(reinterpret_cast<char*>(&len), sizeof(int));
        //create temp buffer to store password +1 to hold \0
        char* temp_buffer = new char[len + 1];
        //password read form file to store in temp buffer
        file.read(temp_buffer, len);
        temp_buffer[len] = '\0';

        password = Secure_String(temp_buffer, len);

        memset(temp_buffer, 0, len + 1);
        delete[] temp_buffer;
    }

};

#endif // !DATA

