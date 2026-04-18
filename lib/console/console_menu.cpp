#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "tool_state.h"
#include "display_common.h"
#include "wifi_common.h"
#include "console_common.h"
#include "registry_attacks.h"

int selected_index = 0;
const char *selected_tag = "";

char *attack_type = "wifi";

void tool_menu_next()
{
    const int menu_size = wifi_attack_count;
    if (menu_size <= 0)
        return;

    selected_index++;

    if (selected_index >= menu_size)
    {
        selected_index = 0;
    }
}

void tool_menu_prev()
{
    const int menu_size = wifi_attack_count;
    if (menu_size <= 0)
        return;

    selected_index--;
    if (selected_index < 0)
    {
        selected_index = menu_size - 1;
    }
}

static int _show_sub_menu(const char *title, const char **options, int option_count)
{
    if (option_count <= 0)
    {
        return -1;
    }

    int selected_option = 0;
    const int items_per_page = 4;

    clear_button_state();

    while (true)
    {
        if (is_movefd_pressed)
        {
            is_movefd_pressed = false;
            selected_option++;
            if (selected_option >= option_count)
            {
                selected_option = 0;
            }
        }

        if (is_movebd_pressed)
        {
            is_movebd_pressed = false;
            selected_option--;
            if (selected_option < 0)
            {
                selected_option = option_count - 1;
            }
        }

        if (is_select_pressed)
        {
            is_select_pressed = false;
            clear_button_state();
            return selected_option;
        }

        if (is_home_pressed)
        {
            is_home_pressed = false;
            clear_button_state();
            return -1;
        }

        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(2, 2);
        display.print(title);
        display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

        int page = selected_option / items_per_page;
        int start = page * items_per_page;
        int end = start + items_per_page;
        if (end > option_count)
        {
            end = option_count;
        }

        int y = 14;
        for (int i = start; i < end; i++)
        {
            if (i == selected_option)
            {
                display.fillRect(0, y, 125, 10, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            }
            else
            {
                display.setTextColor(SSD1306_WHITE);
            }

            display.setCursor(2, y);
            display.print(options[i]);
            y += 11;
        }

        int total_pages = (option_count + items_per_page - 1) / items_per_page;
        int bar_height = 64 / total_pages;
        int bar_y = page * bar_height;
        display.fillRect(125, bar_y, 3, bar_height, SSD1306_WHITE);

        display.display();
        delay(85);
    }
}

static int _run_attack_config_selector(const char *title, ConfigOption *options, int option_count)
{
    if (option_count <= 0)
        return -1;

    int selected_option = 0;
    const int items_per_page = 4;

    clear_button_state();

    while (true)
    {
        // Move forward
        if (is_movefd_pressed)
        {
            is_movefd_pressed = false;
            selected_option++;
            if (selected_option >= option_count)
                selected_option = 0;
        }

        // Move backward
        if (is_movebd_pressed)
        {
            is_movebd_pressed = false;
            selected_option--;
            if (selected_option < 0)
                selected_option = option_count - 1;
        }

        // Toggle selection
        if (is_select_pressed)
        {
            is_select_pressed = false;
            options[selected_option].selected = !options[selected_option].selected;
        }

        // Confirm (MENU button)
        if (is_menu_pressed)
        {
            is_menu_pressed = false;

            int config_mask = 0;

            for (int i = 0; i < option_count; i++)
            {
                if (options[i].selected)
                {
                    config_mask |= options[i].flag;
                }
            }

            clear_button_state();
            return config_mask;
        }

        // 🔙 Back (HOME button)
        if (is_home_pressed)
        {
            is_home_pressed = false;
            clear_button_state();
            return -1; // now works (int type)
        }

        // Render UI
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);

        display.setCursor(2, 1);
        display.print(title);
        display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

        int page = selected_option / items_per_page;
        int start = page * items_per_page;
        int end = start + items_per_page;

        if (end > option_count)
            end = option_count;

        int y = 12;

        for (int i = start; i < end; i++)
        {
            if (i == selected_option)
            {
                display.fillRect(0, y, 125, 10, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            }
            else
            {
                display.setTextColor(SSD1306_WHITE);
            }

            display.setCursor(2, y);
            display.print(options[i].selected ? "[x] " : "[ ] ");
            display.print(options[i].label);

            y += 11;
        }

        // Scrollbar
        int total_pages = (option_count + items_per_page - 1) / items_per_page;
        int bar_height = 64 / total_pages;
        int bar_y = page * bar_height;

        display.fillRect(125, bar_y, 3, bar_height, SSD1306_WHITE);

        print_text("Start: Menu", 5, 55, 1);

        display.display();
        delay(30);
    }
}

void _list_attacks_menu(AttacksList_t *attack_type, int list_count)
{
    const int items_per_page = 5;
    const int menu_size = list_count;

    static int scroll_offset = 0;
    static int last_selected_index = -1;
    static unsigned long last_scroll_time = 0;

    const int SCROLL_SPEED = 150;

    display.clearDisplay();
    display.setTextSize(1);

    // No items case
    if (menu_size == 0)
    {
        display.setCursor(20, 25);
        display.setTextColor(SSD1306_WHITE);
        display.print("No items");
        display.display();
        return;
    }

    // Safety bounds
    if (selected_index >= menu_size)
        selected_index = 0;
    if (selected_index < 0)
        selected_index = 0;

    int page = selected_index / items_per_page;
    int start = page * items_per_page;
    int end = start + items_per_page;

    if (end > menu_size)
        end = menu_size;

    int y = 5;

    for (int i = start; i < end; i++)
    {
        String text = attack_type[i].attack_name;

        if (i == selected_index)
        {
            // Highlight bar
            display.fillRect(0, y - 1, 124, 11, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);

            // Reset scroll when selection changes
            if (selected_index != last_selected_index)
            {
                scroll_offset = 0;
                last_selected_index = selected_index;
            }

            int text_width = text.length() * 6;

            // scroll only if too long
            if (text_width > 116)
            {
                if (millis() - last_scroll_time > SCROLL_SPEED)
                {
                    scroll_offset++;
                    last_scroll_time = millis();

                    if (scroll_offset > text_width + 20)
                        scroll_offset = 0;
                }

                display.setCursor(2 - scroll_offset, y);
                display.print(text);

                // second copy for smooth loop
                display.setCursor(2 - scroll_offset + text_width + 16, y);
                display.print(text);
            }
            else
            {
                display.setCursor(2, y);
                display.print(text);
            }
        }
        else
        {
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(2, y);
            display.print(text);
        }

        y += 12;
    }

    // Right-side page bar + footer hints.
    int total_pages = (menu_size + items_per_page - 1) / items_per_page;
    int bar_height = 64 / total_pages;
    int bar_y = page * bar_height;
    display.fillRect(125, bar_y, 3, bar_height, SSD1306_WHITE);

    display.display();
}

void handle_menu_selection()
{
    if (strcmp(attack_type, "wifi") == 0)
    {
        if (strcmp(selected_tag, "scan_network") == 0)
        {
            const char *scan_modes[] = {"Quick Scan", "Deep Scan", "Hidden Net Scan"};

            int mode_index = _show_sub_menu("SCAN MODE", scan_modes, 2);

            if (mode_index < 0)
            {
                selected_tag = "";
                current_screen = MENU_SCREEN;
                clear_display();
                return;
            }

            ConfigOption scan_options[] = {
                {"Random Mac", CFG_OPT_ONE, false},
                {"Safe Scan", CFG_OPT_TWO, false},
            };
            int scan_config = _run_attack_config_selector("SCAN CONFIG", scan_options, 2);

            if (scan_config < 0)
            {
                current_screen = MENU_SCREEN;
                clear_display();
                return;
            }

            bool random_mac = (scan_config & CFG_OPT_ONE) != 0;
            bool safe_mode = (scan_config & CFG_OPT_TWO) != 0;

            if (mode_index == 1)
            {
                safe_mode = true;
            }

            init_wifi();
            scan_wifi("Scanning WiFi", random_mac, safe_mode);
            disable_wifi();

            selected_tag = "";
            current_screen = RESULT_SCREEN;
        }
        else if (strcmp(selected_tag, "attack_deauth") == 0)
        {
            const char *deauth_modes[] = {
                "Targeted Deauth",
                "Broadcast Deauth",
                "Combo Deauth"};
            int mode_index = _show_sub_menu("DEAUTH MODE", deauth_modes, 3);
            if (mode_index < 0)
            {
                selected_tag = "";
                current_screen = MENU_SCREEN;
                clear_display();
                return;
            }

            ConfigOption deauth_options[] = {
                {"Random MAC", CFG_OPT_ONE, false},
                {"Burst Mode", CFG_OPT_TWO, false},
                {"Safe Mode", CFG_OPT_THREE, false},
                {"Jammer Mode", CFG_OPT_FOUR, false}};

            int config_mask = _run_attack_config_selector("DEAUTH CONFIG", deauth_options, 4);
            if (config_mask < 0)
            {
                selected_tag = "";
                current_screen = MENU_SCREEN;
                clear_display();
                return;
            }

            bool random_mac = (config_mask & CFG_OPT_ONE) != 0;
            bool burst_mode = (config_mask & CFG_OPT_TWO) != 0;
            bool safe_mode = (config_mask & CFG_OPT_THREE) != 0;
            bool jammer_mode = (config_mask & CFG_OPT_FOUR) != 0;

            if (mode_index == 0)
            {
                launch_deauth("targeted_deauth", random_mac, burst_mode, safe_mode, jammer_mode);
            }
            else if (mode_index == 1)
            {
                launch_deauth("broadcast_deauth", random_mac, burst_mode, safe_mode, jammer_mode);
            }
            else
            {
                launch_deauth("combo_deauth", random_mac, burst_mode, safe_mode, jammer_mode);
            }

            selected_tag = "";
            current_screen = MENU_SCREEN;
            clear_display();
        }
        else if (strcmp(selected_tag, "attack_eviltwin") == 0)
        {
            show_task_progress_frame("Evil Twin soon", 100, 0, false);
            delay(400);
            selected_tag = "";
            current_screen = MENU_SCREEN;
            clear_display();
        }
        else if (strcmp(selected_tag, "attack_deauth_eviltwin") == 0)
        {
            launch_deauth("targeted_deauth", false, false, false, false);
            show_task_progress_frame("Evil Twin soon", 100, 0, false);
            delay(400);
            selected_tag = "";
            current_screen = MENU_SCREEN;
            clear_display();
        }
        else
        {
            selected_tag = "";
            current_screen = HOME_SCREEN;
        }
    }
    else if (strcmp(attack_type, "ble") == 0)
    {
        print_text("bluetooth menu selection will be handled later", 1, 1, 1);
    }
}

void handle_result_screen()
{
    if (strcmp(attack_type, "wifi") == 0)
    {
        static int result_selected_index = 0;
        static bool result_show_details = false;

        if (scan_result_type == WIFI_SCAN_R)
        {
            // Reset state only once when new results arrive.
            result_selected_index = 0;
            result_show_details = false;
            scan_result_type = NO_R;
        }

        const int items_per_page = 4;
        int menu_size = scanned_network_count;

        if (menu_size <= 0)
        {
            result_selected_index = 0;
        }
        else
        {
            if (result_selected_index >= menu_size)
                result_selected_index = 0;
            if (result_selected_index < 0)
                result_selected_index = 0;
        }

        if (is_movefd_pressed)
        {
            is_movefd_pressed = false;
            if (!result_show_details && menu_size > 0)
            {
                result_selected_index++;
                if (result_selected_index >= menu_size)
                    result_selected_index = 0;
            }
        }

        if (is_movebd_pressed)
        {
            is_movebd_pressed = false;
            if (!result_show_details && menu_size > 0)
            {
                result_selected_index--;
                if (result_selected_index < 0)
                    result_selected_index = menu_size - 1;
            }
        }

        if (is_select_pressed)
        {
            is_select_pressed = false;
            if (menu_size > 0)
            {
                result_show_details = true;
            }
        }

        if (is_home_pressed)
        {
            is_home_pressed = false;
            if (result_show_details)
            {
                result_show_details = false;
            }
            else
            {
                result_selected_index = 0;
                result_show_details = false;
                current_screen = MENU_SCREEN;
                clear_display();
                return;
            }
        }

        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(2, 1);
        display.print("SCAN RESULT");
        display.drawLine(0, 12, 127, 12, SSD1306_WHITE);

        if (menu_size == 0)
        {
            display.setCursor(2, 25);
            display.print("No networks found");
            display.drawLine(0, 52, 127, 52, SSD1306_WHITE);

            draw_centered_text("H4KNITER", 55, 1);

            display.display();
            return;
        }

        if (result_show_details)
        {
            WiFiNetwork_t net = networks[result_selected_index];

            display.setCursor(2, 16);
            display.print(String("SSID: ") + (net.ssid.length() > 0 ? net.ssid : "<hidden>"));
            display.setCursor(2, 36);
            display.print(String("RSSI:") + net.rssi + String(" dBm"));
            display.setCursor(2, 46);
            display.print("CH:");
            display.print(net.channel);
            display.print(" ");
            display.print(net.security);

            display.display();
            return;
        }

        int page = result_selected_index / items_per_page;
        int start = page * items_per_page;
        int end = start + items_per_page;

        if (end > menu_size)
            end = menu_size;

        int y = 12;
        for (int i = start; i < end; i++)
        {
            String ssid = networks[i].ssid;

            if (i == result_selected_index)
            {
                display.fillRect(0, y, 128, 10, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            }
            else
            {
                display.setTextColor(SSD1306_WHITE);
            }

            display.setCursor(2, y);
            if (ssid.length() > 18)
            {
                display.print(ssid.substring(0, 18));
            }
            else
            {
                display.print(ssid);
            }

            display.setCursor(90, y);
            display.print(networks[i].rssi);

            y += 12;
        }

        int total_pages = (menu_size + items_per_page - 1) / items_per_page;
        int bar_height = 64 / total_pages;
        int bar_y = page * bar_height;
        display.fillRect(125, bar_y, 3, bar_height, SSD1306_WHITE);

        display.display();

    } else if(strcmp(attack_type, "ble") == 0)
    {
        print_text("bluetooth result page will be handled later", 1, 1, 1);
    }
}

void handle_attacks_menu()
{
    if (strcmp(attack_type, "wifi") == 0)
    {
        _list_attacks_menu(WiFiAttacksList, wifi_attack_count);
    }
    else if (strcmp(attack_type, "ble") == 0)
    {
        _list_attacks_menu(BleAttacksList, ble_attack_count);
    }
}