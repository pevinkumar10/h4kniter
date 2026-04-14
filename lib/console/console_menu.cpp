#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "tool_state.h"
#include "display_common.h"
#include "wifi_common.h"
#include "registry_attacks.h"

int selected_index = 0;
const char* selected_tag = "";

void _list_menu()
{
    const int items_per_page = 5;
    const int menu_size = wifi_attack_count;

    static int scroll_offset = 0;
    static unsigned long last_scroll_time = 0;
    static int last_selected_index = -1;

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
    if (selected_index >= menu_size) selected_index = 0;
    if (selected_index < 0) selected_index = 0;

    int page = selected_index / items_per_page;
    int start = page * items_per_page;
    int end = start + items_per_page;

    if (end > menu_size) end = menu_size;

    int y = 5;

    for (int i = start; i < end; i++)
    {
        String text = WiFiAttacksList[i].attack_name;

        if (i == selected_index)
        {
            // Highlight bar
            display.fillRect(0, y - 1, 124, 11, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);

            // Reset scroll when selection changes
            if (selected_index != last_selected_index) {
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

    display.display();
}

void tool_menu_next()
{
    const int menu_size = wifi_attack_count;
    if (menu_size <= 0) return;

    selected_index++;

    if (selected_index >= menu_size)
    {
        selected_index = 0;
    }
}

void handle_selection()
{
    if (strcmp(selected_tag, "back") == 0)
    {
        selected_tag = "";
        current_screen = HOME_SCREEN;
    }
    else if (strcmp(selected_tag, "scan_network") == 0)
    {
        init_wifi();
        scan_wifi("Scanning WiFi");
        disable_wifi();
        
        selected_tag = "";
        current_screen = RESULT_SCREEN;
    }
    else
    {
        selected_tag = "";
        current_screen = HOME_SCREEN;
    }
}

void handle_menu(){
    _list_menu();
}