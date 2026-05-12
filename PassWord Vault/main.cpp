#include <iostream>
#include <string>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "VaultManager.h"
#include "Exceptions.h"

// -----------------------------------------------------------------------
// HELPER FUNCTION: Word Wrapper
// -----------------------------------------------------------------------
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

    // ---------------------------------------------------------
    // HARDWARE TARGET: Lenovo L470 Native (1366 x 768)
    // ---------------------------------------------------------
    InitWindow(1366, 768, "Secure Password Vault");
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

    bool is_secure_note = false;
    char content_buf[512] = "";
    bool content_active = false;

    std::string duplicate_error = "";
    int item_to_show = -1;

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(YELLOW);

        switch (current_screen) {

        case SCREEN_SETUP: {
            // Re-centered for 1366 width
            DrawText("First-Time Setup", 530, 150, 30, DARKBLUE);
            DrawText("Choose a master password for your vault.", 440, 200, 18, DARKGRAY);

            DrawText("New Password:", 430, 260, 20, BLACK);
            if (GuiTextBox(Rectangle{ 640.0f, 255.0f, 260.0f, 35.0f }, setup_pass_buf, 128, setup_pass_active))
                setup_pass_active = !setup_pass_active;

            DrawText("Confirm Password:", 430, 315, 20, BLACK);
            if (GuiTextBox(Rectangle{ 640.0f, 310.0f, 260.0f, 35.0f }, setup_confirm_buf, 128, setup_confirm_active))
                setup_confirm_active = !setup_confirm_active;

            if (GuiButton(Rectangle{ 580.0f, 380.0f, 200.0f, 40.0f }, "CREATE VAULT")) {
                std::string p1(setup_pass_buf);
                std::string p2(setup_confirm_buf);

                if (p1.empty()) setup_error = "Password cannot be empty.";
                else if (p1 != p2) setup_error = "Passwords do not match.";
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
                DrawText(setup_error.c_str(), 480, 440, 20, RED);

            break;
        }

        case SCREEN_LOGIN: {
            // Re-centered for 1366 width
            DrawText("Secure Vault Login", 530, 200, 30, DARKGRAY);

            if (GuiTextBox(Rectangle{ 530.0f, 300.0f, 300.0f, 40.0f }, login_pass_buffer, 128, is_pass_box_active))
                is_pass_box_active = !is_pass_box_active;

            if (GuiButton(Rectangle{ 580.0f, 370.0f, 200.0f, 40.0f }, "LOGIN")) {
                try {
                    std::string typed_pass(login_pass_buffer);
                    if (MyVault.Login(typed_pass)) {
                        MyVault.LoadVault();
                        current_screen = SCREEN_DASHBOARD;
                        error_message = "";
                        item_to_show = -1;
                    }
                }
                catch (AuthException& e) {
                    error_message = e.what();
                    error_message += " (Attempts: " + std::to_string(e.Get_Attempts()) + ")";
                }
            }

            if (!error_message.empty())
                DrawText(error_message.c_str(), 480, 430, 20, RED);

            break;
        }

        case SCREEN_DASHBOARD: {
            DrawText("Vault Dashboard", 50, 30, 30, DARKBLUE);

            // Shifted completely to the right edge
            if (GuiButton(Rectangle{ 1210.0f, 30.0f, 100.0f, 30.0f }, "Logout")) {
                MyVault.Logout();
                current_screen = SCREEN_LOGIN;
                memset(login_pass_buffer, 0, 128);
            }

            if (GuiButton(Rectangle{ 50.0f, 80.0f, 160.0f, 30.0f }, "+ Add New Entry")) {
                duplicate_error = "";
                is_secure_note = false;
                current_screen = SCREEN_ADD_ENTRY;
            }

            DrawText("Title", 50, 130, 18, BLACK);
            DrawText("Username", 200, 130, 18, BLACK);
            DrawText("URL / Content", 380, 130, 18, BLACK);

            // Stretched the header line across the 1366 display
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
                        // Increased word wrap capacity to 90 characters for the wider screen
                        std::string wrapped = WrapText(sn->Get_Content(), 65, lines);

                        DrawText(wrapped.c_str(), 380, y_pos, 18, RED);

                        if (lines > 1) {
                            row_height = 30 + ((lines - 1) * 20);
                        }
                    }
                }
                else {
                    if (arr[i]->Get_Type() == 1) {
                        DrawText(arr[i]->Get_URL().c_str(), 380, y_pos, 18, DARKGRAY);
                    }
                    else {
                        DrawText("[SECURE NOTE]", 380, y_pos, 18, DARKGRAY);
                    }
                }

                // Buttons anchored to the new 1366 right edge
                if (GuiButton(Rectangle{ 1110.0f, (float)y_pos - 4, 72.0f, 24.0f }, "View")) {
                    item_to_show = (item_to_show == i) ? -1 : i;
                }

                if (GuiButton(Rectangle{ 1190.0f, (float)y_pos - 4, 65.0f, 24.0f }, "Delete")) {
                    MyVault.Delete_Entry(i);
                    item_to_show = -1;
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
                if (GuiTextBox(Rectangle{ 150.0f, 165.0f, 300.0f, 30.0f }, user_buf, 128, user_active)) user_active = !user_active;

                DrawText("URL:", 50, 220, 20, BLACK);
                if (GuiTextBox(Rectangle{ 150.0f, 215.0f, 300.0f, 30.0f }, url_buf, 128, url_active)) url_active = !url_active;

                DrawText("Password:", 50, 270, 20, BLACK);
                if (GuiTextBox(Rectangle{ 150.0f, 265.0f, 300.0f, 30.0f }, Pass_buf, 128, Pass_active)) Pass_active = !Pass_active;
            }
            else {
                DrawText("Content:", 50, 170, 20, BLACK);
                // Stretched the Secure Note text box significantly to use the new screen real estate
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

        } // end switch

        EndDrawing();
    }

    MyVault.SaveVault();
    CloseWindow();
    return 0;
}