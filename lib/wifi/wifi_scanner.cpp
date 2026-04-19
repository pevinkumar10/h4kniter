#include "WiFi.h"
#include "wifi_common.h"
#include "display_common.h"
#include "esp_wifi.h"
#include "esp_system.h"

WiFiNetwork_t networks[MAX_NETWORKS];
int scanned_network_count = 0;

void get_bssid(uint8_t *bssid, char *output_buffer);

static void reset_network_cache()
{
    scanned_network_count = 0;

    for (int i = 0; i < MAX_NETWORKS; i++)
    {
        networks[i].ssid = "";
        networks[i].rssi = 0;
        networks[i].channel = 0;
        networks[i].bssid[0] = '\0';
        networks[i].security = "";
    }
}

static void fill_network_cache(int no_of_wifi)
{
    if (no_of_wifi <= 0)
    {
        return;
    }

    int limit = (no_of_wifi > MAX_NETWORKS) ? MAX_NETWORKS : no_of_wifi;
    scanned_network_count = limit;

    for (int net = 0; net < limit; net++)
    {
        uint8_t *bssid_array = WiFi.BSSID(net);
        get_bssid(bssid_array, networks[net].bssid);

        networks[net].ssid = WiFi.SSID(net);
        networks[net].rssi = WiFi.RSSI(net);
        networks[net].channel = WiFi.channel(net);

        switch (WiFi.encryptionType(net))
        {
        case WIFI_AUTH_OPEN:
            networks[net].security = "OPEN";
            break;
        case WIFI_AUTH_WEP:
            networks[net].security = "WEP";
            break;
        case WIFI_AUTH_WPA_PSK:
            networks[net].security = "WPA";
            break;
        case WIFI_AUTH_WPA2_PSK:
            networks[net].security = "WPA2";
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            networks[net].security = "WPA+WPA2";
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            networks[net].security = "WPA2-EAP";
            break;
        case WIFI_AUTH_WPA3_PSK:
            networks[net].security = "WPA3";
            break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
            networks[net].security = "WPA2+WPA3";
            break;
        case WIFI_AUTH_WAPI_PSK:
            networks[net].security = "WAPI";
            break;
        default:
            networks[net].security = "UNKNOWN";
        }

        delay(5);
    }
}

void get_bssid(uint8_t *bssid, char *output_buffer)
{
    snprintf(output_buffer, 18,
             "%02X:%02X:%02X:%02X:%02X:%02X",
             bssid[0], bssid[1], bssid[2],
             bssid[3], bssid[4], bssid[5]);
}

void scan_wifi()
{
    reset_network_cache();
    int no_of_wifi = WiFi.scanNetworks();
    fill_network_cache(no_of_wifi);
}

void scan_wifi(const char* loading_text)
{
    scan_wifi(loading_text, false);
}

void scan_wifi(const char* loading_text, bool safe_mode)
{
    reset_network_cache();

    WiFi.scanDelete();
    
    uint8_t phase = 0;
    uint8_t fake_progress = 0;
    uint16_t scan_delay = 85;
    
    bool show_hidden = true;

    WiFi.scanNetworks(true, show_hidden, safe_mode);

    if (safe_mode) scan_delay = 140;

    while (true)
    {
        int scan_state = WiFi.scanComplete();

        if (scan_state >= 0)
        {
            fill_network_cache(scan_state);
            break;
        }

        if (scan_state == WIFI_SCAN_FAILED)
        {
            break;
        }

        show_task_progress_frame(loading_text, fake_progress, phase, true);

        phase = (phase + 1) % 3;
        if (fake_progress < 92)
        {
            fake_progress += 4;
        }

        delay(scan_delay);
    }

    scan_result_type = WIFI_SCAN_R;

    // Final completion frame for visual closure.
    show_task_progress_frame(loading_text, 100, phase, true);
    delay(120);
}