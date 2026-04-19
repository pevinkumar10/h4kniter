#include "Arduino.h"
#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "wifi_common.h"
#include "esp_wifi.h"

void _generae_random_mac(uint8_t* output_buffer) {

    uint8_t newMACAddress[] = {0x02, 0x01, 0x02, 0x03, 0x04, 0x05};
  
    // Generate random bytes for the last 4 bytes
    for(int i = 2; i < 6; i++) {
        newMACAddress[i] = esp_random() % 256; 
    }

    memcpy(output_buffer, newMACAddress, sizeof(newMACAddress));
}

void set_random_mac(){
    uint8_t mac[6];

    _generae_random_mac(mac);

    esp_wifi_set_mac(WIFI_IF_STA, mac);
}

bool parse_bssid(const char *bssid_str, uint8_t out[6])
{
    if (bssid_str == nullptr)
    {
        return false;
    }

    unsigned int b0, b1, b2, b3, b4, b5;
    int parsed = sscanf(
        bssid_str,
        "%02X:%02X:%02X:%02X:%02X:%02X",
        &b0, &b1, &b2, &b3, &b4, &b5);

    if (parsed != 6)
    {
        return false;
    }

    out[0] = static_cast<uint8_t>(b0);
    out[1] = static_cast<uint8_t>(b1);
    out[2] = static_cast<uint8_t>(b2);
    out[3] = static_cast<uint8_t>(b3);
    out[4] = static_cast<uint8_t>(b4);
    out[5] = static_cast<uint8_t>(b5);
    return true;
}

int select_target_network()
{
    if (scanned_network_count <= 0)
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
            if (selected_option >= scanned_network_count)
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
                selected_option = scanned_network_count - 1;
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
        display.print("TARGET AP");
        display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

        int page = selected_option / items_per_page;
        int start = page * items_per_page;
        int end = start + items_per_page;
        if (end > scanned_network_count)
        {
            end = scanned_network_count;
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
            String ssid = networks[i].ssid.length() > 0 ? networks[i].ssid : "<hidden>";
            if (ssid.length() > 12)
            {
                ssid = ssid.substring(0, 12);
            }
            display.print(ssid);
            display.setCursor(82, y);
            display.print(networks[i].rssi);
            y += 11;
        }

        int total_pages = (scanned_network_count + items_per_page - 1) / items_per_page;
        int bar_height = 64 / total_pages;
        int bar_y = page * bar_height;
        display.fillRect(125, bar_y, 3, bar_height, SSD1306_WHITE);

        display.display();
        delay(30);
    }
}