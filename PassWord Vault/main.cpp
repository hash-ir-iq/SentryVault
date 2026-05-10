#include"PasswordString.h"
#include<iostream>
#include<cstring>
#include<fstream>

using namespace std;
int main()
{
	int choice = 1;
	string MasterKey;
	

	cout << "Welcome to the password vault:-\n\n";
	cout << "First time user?\n";	//actually abhi ise dekhna hai ke how to handle it
	cout << "\t\t\t\tEnter the Master Password\n";
	
	//If password success, proceed to the following stage:

	while (choice) {
		if (choice != 1 || choice != 2 || choice != 3) {
			cout << "Enter the correct choice\n";
		}
		cout << "1. Display Passwords\n2. Add new Password\n3. Delete a Password\n0. Lock and Exit\n";//will replace this when adding GUI
		switch (choice) {
			case 1:
				//Call display functions
				break;
			case 2:
				// new password calling, array resize, secure string and writing data all called again 
				break;
			case 3:
				// array resizing and also shifting will happen
			case 0:
				
		}

	}

}


