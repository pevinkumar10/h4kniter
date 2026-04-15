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
void scan_wifi(const char* loading_text);
void scan_wifi(const char* loading_text, bool random_mac, bool safe_mode);
void disable_wifi();

// WiFi Attacks
void launch_deauth(const char *attack_type, bool random_mac = false, bool burst_mode = false, bool safe_mode = false, bool jammer_mode = false);