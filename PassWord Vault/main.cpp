#include <iostream>
#include "raylib.h"

#define _CRT_SECURE_NO_WARNINGS 

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "VaultManager.h"
#include "Exceptions.h"

// enum to track which page we are looking at
enum GUI_State {    
    SCREEN_LOGIN,
    SCREEN_DASHBOARD,
    SCREEN_ADD_ENTRY
};

int main() {
    // creating the main vault brain
    VaultManager MyVault;

    // dummy setup for the master password
    MyVault.Create_Vault("FAST123");

    // starting the window at 800x600 resolution
    InitWindow(800, 600, "Secure Password Vault");
    SetTargetFPS(60);

    // we start on the login page
    GUI_State current_screen = SCREEN_LOGIN;

    // variables for the login text box
    char login_pass_buffer[128] = "";
    bool is_pass_box_active = false;
    std::string error_message = "";

    // variables for the add entry text boxes
    char title_buf[128] = "";
    bool title_active = false;
    char user_buf[128] = "";
    bool user_active = false;
    char url_buf[128] = "";
    bool url_active = false;
    char Pass_buf[128] = "";
    bool Pass_active = false;

    // the main game loop that keeps the app running
    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // checking which screen we should draw right now
        switch (current_screen) {

        case SCREEN_LOGIN: {
            DrawText("Secure Vault Login", 250, 150, 30, DARKGRAY);

            // drawing the text box so user can type master password
            if (GuiTextBox(Rectangle{ 250.0f, 250.0f, 300.0f, 40.0f }, login_pass_buffer, 128, is_pass_box_active)) {
                is_pass_box_active = !is_pass_box_active;
            }

            // drawing the login button
            if (GuiButton(Rectangle{ 300.0f, 320.0f, 200.0f, 40.0f }, "LOGIN")) {
                try {
                    std::string typed_pass(login_pass_buffer);

                    // attempting to log in with the text from the box
                    if (MyVault.Login(typed_pass)) {
                        MyVault.LoadVault(); // load data from the .dat file
                        current_screen = SCREEN_DASHBOARD; // switch the page
                        error_message = "";
                    }
                }
                catch (AuthException& e) {
                    // catching the custom exception and updating the UI text
                    error_message = e.what();
                    error_message += " (Attempts: " + std::to_string(e.Get_Attempts()) + ")";
                }
            }

            // drawing the error message in red if they typed the wrong password
            if (error_message != "") {
                DrawText(error_message.c_str(), 250, 380, 20, RED);
            }

            break;
        }

        case SCREEN_DASHBOARD: {
            DrawText("Vault Dashboard", 50, 30, 30, DARKBLUE);

            // logout button in the top right
            if (GuiButton(Rectangle{ 650.0f, 30.0f, 100.0f, 30.0f }, "Logout")) {
                MyVault.Logout();
                current_screen = SCREEN_LOGIN;

                // wipe the password buffer so it's empty when they come back
                memset(login_pass_buffer, 0, 128);
            }

            // button to switch to the add new entry screen
            if (GuiButton(Rectangle{ 50.0f, 80.0f, 150.0f, 30.0f }, "+ Add New Password")) {
                current_screen = SCREEN_ADD_ENTRY;
            }

            // drawing the column headers
            DrawText("Title", 50, 130, 20, BLACK);
            DrawText("Username", 200, 130, 20, BLACK);
            DrawText("URL", 400, 130, 20, BLACK);
            DrawLine(50, 155, 750, 155, GRAY);

            // getting the array from the manager
            DynamicArray<Vault_Record*>& arr = MyVault.GetArray();
            int total_items = arr.Get_Size();

            // looping through the array to draw every password
            int y_pos = 170;
            for (int i = 0; i < total_items; i++) {

                // using polymorphism so we don't have to dynamic_cast
                DrawText(arr[i]->Get_Title().c_str(), 50, y_pos, 20, DARKGRAY);
                DrawText(arr[i]->Get_Username().c_str(), 200, y_pos, 20, DARKGRAY);
                DrawText(arr[i]->Get_URL().c_str(), 400, y_pos, 20, DARKGRAY);

                // draw the delete button for this specific item
                if (GuiButton(Rectangle{ 650.0f, (float)y_pos - 5, 80.0f, 25.0f }, "Delete")) {
                    MyVault.Delete_Entry(i);

                    // break out of the for-loop because the array size just changed
                    // and we don't want to crash
                    break;
                }

                y_pos += 30; // moving down 30 pixels for the next row
            }

            break;
        }

        case SCREEN_ADD_ENTRY: {
            DrawText("Add New Entry", 50, 30, 30, DARKBLUE);

            // title input
            DrawText("Title:", 50, 100, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 95.0f, 300.0f, 30.0f }, title_buf, 128, title_active)) title_active = !title_active;

            // username input
            DrawText("Username:", 50, 150, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 145.0f, 300.0f, 30.0f }, user_buf, 128, user_active)) user_active = !user_active;

            // url input
            DrawText("URL:", 50, 200, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 195.0f, 300.0f, 30.0f }, url_buf, 128, url_active)) url_active = !url_active;

            // password input
            DrawText("Password:", 50, 250, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 245.0f, 300.0f, 30.0f }, Pass_buf, 128, Pass_active)) Pass_active = !Pass_active;

            // save button
            if (GuiButton(Rectangle{ 150.0f, 300.0f, 120.0f, 40.0f }, "SAVE")) {

                // converting the typed text arrays into std::strings
                std::string t(title_buf);
                std::string u(user_buf);
                std::string url(url_buf);

                // creating the secure string for the password
                Secure_String p(Pass_buf, strlen(Pass_buf));

                // making a new pointer on the heap
                PasswordEntry* new_entry = new PasswordEntry(t, u, url, p);

                // throwing it into the array
                MyVault.Add_Entry(new_entry);

                // clearing the text boxes so they are empty for the next time
                memset(title_buf, 0, 128);
                memset(user_buf, 0, 128);
                memset(url_buf, 0, 128);
                memset(Pass_buf, 0, 128);

                // return to dashboard
                current_screen = SCREEN_DASHBOARD;
            }

            // cancel button
            if (GuiButton(Rectangle{ 290.0f, 300.0f, 120.0f, 40.0f }, "CANCEL")) {
                current_screen = SCREEN_DASHBOARD;
            }

            break;
        }
        }

        EndDrawing();
    }

    // save everything to the hard drive before the program actually shuts down
    MyVault.SaveVault();

    // give memory back to the OS
    CloseWindow();

    return 0;
}