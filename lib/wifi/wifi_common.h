#pragma once
#include <Arduino.h>

#define MAX_NETWORKS 20

struct WiFiNetwork_t {
    String ssid;
    int32_t rssi;
    int32_t channel;
    char bssid[18];
    String security;
};

extern WiFiNetwork_t networks[MAX_NETWORKS];
extern int scanned_network_count;

void init_wifi();
void scan_wifi();
void disable_wifi();
void scan_wifi(const char* loading_text);
void scan_wifi(const char* loading_text, bool safe_mode);

bool create_ap(const char* ap_name);

// tools specific functions
int select_target_network();
bool parse_bssid(const char *bssid_str, uint8_t out[6]);

// WiFi Attacks
void set_random_mac();
void launch_deauth_handler(const char *attack_type, bool random_mac = false, bool safe_mode = true, bool jammer_mode = false);
void launch_eviltwin(const char *attack_type, bool random_mac = false, bool safe_mode = true);