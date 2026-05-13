#include <iostream>
#include <string>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "VaultManager.h"
#include "Exceptions.h"


std::string WrapText(const std::string& text, int max_chars, int& line_count) {
    std::string wrapped = "";
    int current_len = 0;
    line_count = 1;

    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            wrapped += text[i];
            current_len = 0;
            line_count++;
            continue;
        }

        if (current_len >= max_chars && text[i] == ' ') {
            wrapped += '\n';
            current_len = 0;
            line_count++;
            continue;
        }

        wrapped += text[i];
        current_len++;

        if (current_len >= max_chars + 15) {
            wrapped += "-\n";
            current_len = 0;
            line_count++;
        }
    }
    return wrapped;
}

// SCREEN WELCOME 
enum GUI_State {
    SCREEN_WELCOME,
    SCREEN_SETUP,
    SCREEN_LOGIN,
    SCREEN_DASHBOARD,
    SCREEN_ADD_ENTRY
};

int main() {
    VaultManager MyVault;

    InitWindow(1366, 768, "Secure Password Vault - Multi-User");
    SetTargetFPS(60);

   
    GUI_State current_screen = SCREEN_WELCOME;

    //Login
    char login_user_buffer[128] = "";
    bool is_login_user_active = false;
    char login_pass_buffer[128] = "";
    bool is_pass_box_active = false;
    std::string error_message = "";

    //Setup 
    char setup_user_buf[128] = "";
    bool setup_user_active = false;
    char setup_pass_buf[128] = "";
    bool setup_pass_active = false;
    char setup_confirm_buf[128] = "";
    bool setup_confirm_active = false;
    std::string setup_error = "";

    // Entry
    char title_buf[128] = "";
    bool title_active = false;
    char user_buf[128] = "";
    bool user_active = false;
    char url_buf[128] = "";
    bool url_active = false;
    char Pass_buf[128] = "";
    bool Pass_active = false;
    bool is_secure_note = false;
    char content_buf[512] = "";
    bool content_active = false;

    std::string duplicate_error = "";
    int item_to_show = -1;
    int copied_item_index = -1;

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(YELLOW);

        switch (current_screen) {

        case SCREEN_WELCOME: {
            DrawText("Welcome to Secure Vault", 500, 200, 30, DARKBLUE);
            DrawText("Select an option to proceed.", 540, 250, 18, DARKGRAY);

            if (GuiButton(Rectangle{ 530.0f, 320.0f, 300.0f, 50.0f }, "LOGIN TO EXISTING VAULT")) {
                current_screen = SCREEN_LOGIN;
            }

            if (GuiButton(Rectangle{ 530.0f, 390.0f, 300.0f, 50.0f }, "CREATE NEW VAULT")) {
                current_screen = SCREEN_SETUP;
            }
            break;
        }

        case SCREEN_SETUP: {
            DrawText("Create a New Vault", 530, 100, 30, DARKBLUE);

            DrawText("Vault Username:", 430, 180, 20, BLACK);
            if (GuiTextBox(Rectangle{ 640.0f, 175.0f, 260.0f, 35.0f }, setup_user_buf, 128, setup_user_active))
                setup_user_active = !setup_user_active;

            DrawText("Master Password:", 430, 235, 20, BLACK);
            if (GuiTextBox(Rectangle{ 640.0f, 230.0f, 260.0f, 35.0f }, setup_pass_buf, 128, setup_pass_active))
                setup_pass_active = !setup_pass_active;

            DrawText("Confirm Password:", 430, 290, 20, BLACK);
            if (GuiTextBox(Rectangle{ 640.0f, 285.0f, 260.0f, 35.0f }, setup_confirm_buf, 128, setup_confirm_active))
                setup_confirm_active = !setup_confirm_active;

            if (GuiButton(Rectangle{ 580.0f, 360.0f, 200.0f, 40.0f }, "CREATE VAULT")) {
                std::string u(setup_user_buf);
                std::string p1(setup_pass_buf);
                std::string p2(setup_confirm_buf);

                if (u.empty())
                    setup_error = "Username cannot be empty.";
                else if (p1.empty())
                    setup_error = "Password cannot be empty.";
                else if (p1 != p2)
                    setup_error = "Passwords do not match.";
                else {
                    MyVault.Create_Vault(u, p1);
                    MyVault.SaveVault();

                    memset(setup_user_buf, 0, 128);
                    memset(setup_pass_buf, 0, 128);
                    memset(setup_confirm_buf, 0, 128);
                    setup_error = "";
                    current_screen = SCREEN_DASHBOARD;
                }
            }

            if (GuiButton(Rectangle{ 580.0f, 420.0f, 200.0f, 30.0f }, "BACK")) {
                current_screen = SCREEN_WELCOME;
            }

            if (!setup_error.empty()) 
                DrawText(setup_error.c_str(), 480, 470, 20, RED);
            break;
        }

        case SCREEN_LOGIN: {
            DrawText("Secure Vault Login", 530, 150, 30, DARKGRAY);

            DrawText("Username:", 430, 240, 20, BLACK);
            if (GuiTextBox(Rectangle{ 550.0f, 235.0f, 300.0f, 40.0f }, login_user_buffer, 128, is_login_user_active))
                is_login_user_active = !is_login_user_active;

            DrawText("Password:", 430, 310, 20, BLACK);
            if (GuiTextBox(Rectangle{ 550.0f, 305.0f, 300.0f, 40.0f }, login_pass_buffer, 128, is_pass_box_active))
                is_pass_box_active = !is_pass_box_active;

            if (GuiButton(Rectangle{ 580.0f, 380.0f, 200.0f, 40.0f }, "LOGIN")) {
                try {
                    std::string typed_user(login_user_buffer);
                    std::string typed_pass(login_pass_buffer);

                    if (MyVault.Login(typed_user, typed_pass)) {
                        MyVault.LoadVault();
                        current_screen = SCREEN_DASHBOARD;
                        error_message = "";
                        item_to_show = -1;
                        copied_item_index = -1;
                    }
                }
                catch (AuthException& e) {
                    error_message = e.what();
                    if (e.Get_Attempts() > 0) 
                        error_message += " (Attempts: " + std::to_string(e.Get_Attempts()) + ")";
                }
            }

            if (GuiButton(Rectangle{ 580.0f, 440.0f, 200.0f, 30.0f }, "BACK")) {
                current_screen = SCREEN_WELCOME;
                error_message = "";
            }

            if (!error_message.empty()) DrawText(error_message.c_str(), 480, 490, 20, RED);
            break;
        }

        case SCREEN_DASHBOARD: {
            DrawText("Vault Dashboard", 50, 30, 30, DARKBLUE);

            if (GuiButton(Rectangle{ 1210.0f, 30.0f, 100.0f, 30.0f }, "Logout")) {
                MyVault.Logout(); // Saves file and triggers the RAM scrub
                current_screen = SCREEN_WELCOME;
                memset(login_pass_buffer, 0, 128);
                memset(login_user_buffer, 0, 128);
            }

            if (GuiButton(Rectangle{ 50.0f, 80.0f, 160.0f, 30.0f }, "+ Add New Entry")) {
                duplicate_error = "";
                is_secure_note = false;
                copied_item_index = -1;
                current_screen = SCREEN_ADD_ENTRY;
            }

            DrawText("Title", 50, 130, 18, BLACK);
            DrawText("Username", 200, 130, 18, BLACK);
            DrawText("URL / Content", 380, 130, 18, BLACK);

            DrawLine(50, 152, 1310, 152, GRAY);

            DynamicArray<Vault_Record*>& arr = MyVault.GetArray();
            int total_items = arr.Get_Size();
            int y_pos = 165;

            for (int i = 0; i < total_items; i++) {
                int row_height = 30;

                DrawText(arr[i]->Get_Title().c_str(), 50, y_pos, 18, DARKGRAY);

                if (arr[i]->Get_Type() == 1) {
                    DrawText(arr[i]->Get_Username().c_str(), 200, y_pos, 18, DARKGRAY);
                }
                else {
                    DrawText("---", 200, y_pos, 18, LIGHTGRAY);
                }

                if (item_to_show == i) {
                    if (arr[i]->Get_Type() == 1) {
                        PasswordEntry* pe = dynamic_cast<PasswordEntry*>(arr[i]);
                        DrawText(pe->Get_Password().Get_SecureString_Ptr(), 380, y_pos, 18, RED);
                    }
                    else if (arr[i]->Get_Type() == 2) {
                        SecureNote* sn = dynamic_cast<SecureNote*>(arr[i]);

                        int lines = 1;
                        std::string wrapped = WrapText(sn->Get_Content(), 65, lines);
                        DrawText(wrapped.c_str(), 380, y_pos, 18, RED);
                        if (lines > 1)
                            row_height = 30 + ((lines - 1) * 20);
                    }
                }
                else {
                    if (arr[i]->Get_Type() == 1)
                        DrawText(arr[i]->Get_URL().c_str(), 380, y_pos, 18, DARKGRAY);
                    else
                        DrawText("[SECURE NOTE]", 380, y_pos, 18, DARKGRAY);
                }

                const char* copy_btn_text = (copied_item_index == i) ? "Copied!" : "Copy";
                if (GuiButton(Rectangle{ 1030.0f, (float)y_pos - 4, 72.0f, 24.0f }, copy_btn_text)) {
                    if (arr[i]->Get_Type() == 1) {
                        PasswordEntry* pe = dynamic_cast<PasswordEntry*>(arr[i]);
                        SetClipboardText(pe->Get_Password().Get_SecureString_Ptr());
                    }
                    else if (arr[i]->Get_Type() == 2) {
                        SecureNote* sn = dynamic_cast<SecureNote*>(arr[i]);
                        SetClipboardText(sn->Get_Content().c_str());
                    }
                    copied_item_index = i;
                }

                if (GuiButton(Rectangle{ 1110.0f, (float)y_pos - 4, 72.0f, 24.0f }, "View")) {
                    item_to_show = (item_to_show == i) ? -1 : i;
                }

                if (GuiButton(Rectangle{ 1190.0f, (float)y_pos - 4, 65.0f, 24.0f }, "Delete")) {
                    MyVault.Delete_Entry(i);
                    item_to_show = -1;
                    copied_item_index = -1;
                    break;
                }
                y_pos += row_height;
            }
            break;
        }

        case SCREEN_ADD_ENTRY: {
            DrawText("Add New Entry", 50, 30, 30, DARKBLUE);

            GuiCheckBox(Rectangle{ 50.0f, 75.0f, 20.0f, 20.0f }, " Create Secure Note instead of Password", &is_secure_note);

            DrawText("Title:", 50, 120, 20, BLACK);
            if (GuiTextBox(Rectangle{ 150.0f, 115.0f, 300.0f, 30.0f }, title_buf, 128, title_active))
                title_active = !title_active;

            if (!is_secure_note) {
                DrawText("Username:", 50, 170, 20, BLACK);
                if (GuiTextBox(Rectangle{ 150.0f, 165.0f, 300.0f, 30.0f }, user_buf, 128, user_active)) 
                    user_active = !user_active;

                DrawText("URL:", 50, 220, 20, BLACK);
                if (GuiTextBox(Rectangle{ 150.0f, 215.0f, 300.0f, 30.0f }, url_buf, 128, url_active))
                    url_active = !url_active;

                DrawText("Password:", 50, 270, 20, BLACK);
                if (GuiTextBox(Rectangle{ 150.0f, 265.0f, 300.0f, 30.0f }, Pass_buf, 128, Pass_active))
                    Pass_active = !Pass_active;
            }
            else {
                DrawText("Content:", 50, 170, 20, BLACK);
                if (GuiTextBox(Rectangle{ 150.0f, 165.0f, 800.0f, 120.0f }, content_buf, 512, content_active))
                    content_active = !content_active;
            }

            if (GuiButton(Rectangle{ 150.0f, 330.0f, 120.0f, 40.0f }, "SAVE")) {
                try {
                    Vault_Record* new_entry = nullptr;

                    if (!is_secure_note) {
                        Secure_String p(Pass_buf, (int)strlen(Pass_buf));
                        new_entry = new PasswordEntry(std::string(title_buf), std::string(user_buf), std::string(url_buf), p);
                    }
                    else {
                        new_entry = new SecureNote(std::string(title_buf), std::string(content_buf));
                    }

                    MyVault.Add_Entry(new_entry);

                    memset(title_buf, 0, 128);
                    memset(user_buf, 0, 128);
                    memset(url_buf, 0, 128);
                    memset(Pass_buf, 0, 128);
                    memset(content_buf, 0, 512);

                    duplicate_error = "";
                    item_to_show = -1;
                    current_screen = SCREEN_DASHBOARD;
                }
                catch (DuplicateEntryException& e) {
                    duplicate_error = e.what();
                }
            }

            if (GuiButton(Rectangle{ 290.0f, 330.0f, 120.0f, 40.0f }, "CANCEL")) {
                duplicate_error = "";
                current_screen = SCREEN_DASHBOARD;
            }

            if (!duplicate_error.empty()) {
                DrawText(duplicate_error.c_str(), 150, 390, 20, RED);
            }

            break;
        }

        } 
        EndDrawing();
    }

    MyVault.SaveVault();
    CloseWindow();
    return 0;
}