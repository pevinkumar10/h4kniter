#include "Arduino.h"

#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "interrupts_common.h"
#include "wifi_common.h"

volatile bool is_menu_pressed = false;
volatile bool is_home_pressed = false;
volatile bool is_select_pressed = false;

ToolScreen current_screen = HOME_SCREEN;
static int result_selected_index = 0;
static bool result_show_details = false;

void setup()
{
    init_display();

    Serial.begin(115200);
    // show_loading_bar("BOOTING",1,5);

    setup_button_irq(MENU_BTN_PIN, menu_btn_irq);
    setup_button_irq(BACK_BTN_PIN, back_btn_irq);
    setup_button_irq(SELECT_BTN_PIN, select_btn_irq);

    clear_display();
}

void loop()
{
    update_btns_status();
    static ToolScreen last_screen = HOME_SCREEN;

    if (current_screen == RESULT_SCREEN && last_screen != RESULT_SCREEN)
    {
        result_selected_index = 0;
        result_show_details = false;
    }

    switch (current_screen)
    {

    case MENU_SCREEN:
        handle_menu();
        break;

    case HOME_SCREEN:
        handle_home();
        break;

    case SELECT_SCREEN:
        handle_selection();
        break;

    case DEFAULT_SCREEN:
        print_text("DEFAULT", 1, 1, 1);
        break;

    case RESULT_SCREEN:
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

        // Button actions for result screen:
        // MENU: next AP in list
        // SELECT: open details for selected AP
        // BACK: close details first, then leave result to menu
        if (is_menu_pressed)
        {
            is_menu_pressed = false;
            if (!result_show_details && menu_size > 0)
            {
                result_selected_index++;
                if (result_selected_index >= menu_size)
                    result_selected_index = 0;
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
                current_screen = MENU_SCREEN;
                clear_display();
                break;
            }
        }

        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);

        // Header
        display.setCursor(2, 1);
        display.print("SCAN RESULT");
        display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

        if (menu_size == 0)
        {
            display.setCursor(2, 25);
            display.print("No networks found");
            display.drawLine(0, 54, 127, 54, SSD1306_WHITE);
            display.setCursor(2, 56);
            display.print("BACK: menu");
            display.display();
            break;
        }

        if (result_show_details)
        {
            WiFiNetwork_t net = networks[result_selected_index];

            display.setCursor(2, 14);
            display.print("SSID:");
            display.setCursor(2, 24);
            display.print(net.ssid.length() > 0 ? net.ssid : "<hidden>");

            display.setCursor(2, 34);
            display.print("RSSI:");
            display.print(net.rssi);
            display.print(" dBm");

            display.setCursor(2, 44);
            display.print("CH:");
            display.print(net.channel);
            display.print(" ");
            display.print(net.security);

            display.drawLine(0, 54, 127, 54, SSD1306_WHITE);
            display.setCursor(2, 56);
            display.print("BACK:list");
            display.display();
            break;
        }

        // Paging
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

            // Trim long SSID (optional)
            if (ssid.length() > 18)
            {
                display.print(ssid.substring(0, 18));
            }
            else
            {
                display.print(ssid);
            }

            // Show RSSI on right side
            display.setCursor(90, y);
            display.print(networks[i].rssi);

            y += 12;
        }

        // Scrollbar (optional but nice)
        int total_pages = (menu_size + items_per_page - 1) / items_per_page;
        int bar_height = 64 / total_pages;
        int bar_y = page * bar_height;

        display.fillRect(125, bar_y, 3, bar_height, SSD1306_WHITE);
        display.drawLine(0, 54, 124, 54, SSD1306_WHITE);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(2, 56);
        display.print("M:NEXT  S:OPEN  B:MENU");

        display.display();

        break;
    }

    last_screen = current_screen;
    delay(150);
}