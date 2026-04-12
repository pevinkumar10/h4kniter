#include <WiFi.h>
#include <wifi_common.h>

WiFiNetwork_t networks[MAX_NETWORKS];

void get_bssid(uint8_t *bssid, char *output_buffer)
{
    snprintf(output_buffer, 18,
             "%02X:%02X:%02X:%02X:%02X:%02X",
             bssid[0], bssid[1], bssid[2],
             bssid[3], bssid[4], bssid[5]);
}

void scan_wifi()
{
    int no_of_wifi = WiFi.scanNetworks();

    if (no_of_wifi == 0)
    {
        return;
    }

    int limit = (no_of_wifi > MAX_NETWORKS) ? MAX_NETWORKS : no_of_wifi;

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