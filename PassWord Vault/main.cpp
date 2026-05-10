#include <iostream>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "VaultManager.h"
#include "Exceptions.h"

enum GUI_State {
    SCREEN_SETUP,
    SCREEN_LOGIN,
    SCREEN_DASHBOARD,
    SCREEN_ADD_ENTRY
};

int main() {
    VaultManager MyVault;

    bool vault_exists = false;
    {
        std::ifstream check("vault.dat", std::ios::binary);
        vault_exists = check.good();
    }

    if (vault_exists)
        MyVault.LoadHash();

    InitWindow(800, 600, "Secure Password Vault");
    SetTargetFPS(60);

    GUI_State current_screen = vault_exists ? SCREEN_LOGIN : SCREEN_SETUP;

    char login_pass_buffer[128] = "";
    bool is_pass_box_active = false;
    std::string error_message = "";

    char setup_pass_buf[128] = "";
    bool setup_pass_active = false;
    char setup_confirm_buf[128] = "";
    bool setup_confirm_active = false;
    std::string setup_error = "";

    char title_buf[128] = "";
    bool title_active = false;
    char user_buf[128] = "";
    bool user_active = false;
    char url_buf[128] = "";
    bool url_active = false;
    char Pass_buf[128] = "";
    bool Pass_active = false;

    // Tracker for the View Password feature
    int item_to_show = -1;

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (current_screen) {

        case SCREEN_SETUP: {
            DrawText("First-Time Setup", 250, 100, 30, DARKBLUE);
            DrawText("Choose a master password for your vault.", 160, 150, 18, DARKGRAY);

            DrawText("New Password:", 150, 210, 20, BLACK);
            // Shifted X from 310 to 360
            if (GuiTextBox(Rectangle{ 360.0f, 205.0f, 260.0f, 35.0f }, setup_pass_buf, 128, setup_pass_active))
                setup_pass_active = !setup_pass_active;

            DrawText("Confirm Password:", 150, 265, 20, BLACK);
            // Shifted X from 310 to 360
            if (GuiTextBox(Rectangle{ 360.0f, 260.0f, 260.0f, 35.0f }, setup_confirm_buf, 128, setup_confirm_active))
                setup_confirm_active = !setup_confirm_active;

            // Centered the button relative to the new text box positions
            if (GuiButton(Rectangle{ 330.0f, 320.0f, 200.0f, 40.0f }, "CREATE VAULT")) {
                std::string p1(setup_pass_buf);
                std::string p2(setup_confirm_buf);

                if (p1.empty()) {
                    setup_error = "Password cannot be empty.";
                }
                else if (p1 != p2) {
                    setup_error = "Passwords do not match.";
                }
                else {
                    MyVault.Create_Vault(p1);
                    MyVault.SaveVault();

                    memset(setup_pass_buf, 0, 128);
                    memset(setup_confirm_buf, 0, 128);
                    setup_error = "";

                    current_screen = SCREEN_DASHBOARD;
                }
            }

            if (!setup_error.empty())
                DrawText(setup_error.c_str(), 200, 380, 20, RED);

            break;
        }

        case SCREEN_LOGIN: {
            DrawText("Secure Vault Login", 250, 150, 30, DARKGRAY);

            if (GuiTextBox(Rectangle{ 250.0f, 250.0f, 300.0f, 40.0f }, login_pass_buffer, 128, is_pass_box_active))
                is_pass_box_active = !is_pass_box_active;

            if (GuiButton(Rectangle{ 300.0f, 320.0f, 200.0f, 40.0f }, "LOGIN")) {
                try {
                    std::string typed_pass(login_pass_buffer);

                    if (MyVault.Login(typed_pass)) {
                        MyVault.LoadVault();
                        current_screen = SCREEN_DASHBOARD;
                        error_message = "";
                        item_to_show = -1; // reset view state
                    }
                }
                catch (AuthException& e) {
                    error_message = e.what();
                    error_message += " (Attempts: " + std::to_string(e.Get_Attempts()) + ")";
                }
            }

            if (!error_message.empty())
                DrawText(error_message.c_str(), 200, 380, 20, RED);

            break;
        }

        case SCREEN_DASHBOARD: {
            DrawText("Vault Dashboard", 50, 30, 30, DARKBLUE);

            if (GuiButton(Rectangle{ 650.0f, 30.0f, 100.0f, 30.0f }, "Logout")) {
                MyVault.Logout();
                current_screen = SCREEN_LOGIN;
                memset(login_pass_buffer, 0, 128);
            }

            if (GuiButton(Rectangle{ 50.0f, 80.0f, 160.0f, 30.0f }, "+ Add New Password")) {
                current_screen = SCREEN_ADD_ENTRY;
            }

            DrawText("Title", 50, 130, 18, BLACK);
            DrawText("Username", 200, 130, 18, BLACK);
            DrawText("URL / Password", 380, 130, 18, BLACK);
            DrawLine(50, 152, 760, 152, GRAY);

            DynamicArray<Vault_Record*>& arr = MyVault.GetArray();
            int total_items = arr.Get_Size();

            int y_pos = 165;
            for (int i = 0; i < total_items; i++) {
                DrawText(arr[i]->Get_Title().c_str(), 50, y_pos, 18, DARKGRAY);
                DrawText(arr[i]->Get_Username().c_str(), 200, y_pos, 18, DARKGRAY);

                // VISIBILITY LOGIC: Swap URL for Password if this entry is clicked
                if (item_to_show == i) {
                    PasswordEntry* pe = dynamic_cast<PasswordEntry*>(arr[i]);
                    if (pe) {
                        DrawText(pe->Get_Password().Get_SecureString_Ptr(), 380, y_pos, 18, RED);
                    }
                }
                else {
                    DrawText(arr[i]->Get_URL().c_str(), 380, y_pos, 18, DARKGRAY);
                }

                // View PW Button: Toggles the item_to_show integer
                if (GuiButton(Rectangle{ 560.0f, (float)y_pos - 4, 72.0f, 24.0f }, "View PW")) {
                    item_to_show = (item_to_show == i) ? -1 : i;
                }

                if (GuiButton(Rectangle{ 640.0f, (float)y_pos - 4, 65.0f, 24.0f }, "Delete")) {
                    MyVault.Delete_Entry(i);
                    item_to_show = -1; // reset view if something is deleted to prevent out of bounds
                    break;
                }

                y_pos += 30;
            }

            break;
        }

        case SCREEN_ADD_ENTRY: {
            DrawText("Add New Entry", 50, 30, 30, DARKBLUE);

            DrawText("Title:", 50, 100, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 95.0f, 300.0f, 30.0f }, title_buf, 128, title_active))
                title_active = !title_active;

            DrawText("Username:", 50, 150, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 145.0f, 300.0f, 30.0f }, user_buf, 128, user_active))
                user_active = !user_active;

            DrawText("URL:", 50, 200, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 195.0f, 300.0f, 30.0f }, url_buf, 128, url_active))
                url_active = !url_active;

            DrawText("Password:", 50, 250, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 245.0f, 300.0f, 30.0f }, Pass_buf, 128, Pass_active))
                Pass_active = !Pass_active;

            if (GuiButton(Rectangle{ 150.0f, 300.0f, 120.0f, 40.0f }, "SAVE")) {
                Secure_String p(Pass_buf, (int)strlen(Pass_buf));
                PasswordEntry* new_entry = new PasswordEntry(
                    std::string(title_buf),
                    std::string(user_buf),
                    std::string(url_buf),
                    p
                );
                MyVault.Add_Entry(new_entry);

                memset(title_buf, 0, 128);
                memset(user_buf, 0, 128);
                memset(url_buf, 0, 128);
                memset(Pass_buf, 0, 128);

                item_to_show = -1; // reset view state
                current_screen = SCREEN_DASHBOARD;
            }

            if (GuiButton(Rectangle{ 290.0f, 300.0f, 120.0f, 40.0f }, "CANCEL"))
                current_screen = SCREEN_DASHBOARD;

            break;
        }

        } // end switch

        EndDrawing();
    }

    MyVault.SaveVault();
    CloseWindow();
    return 0;
}