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
void disable_wifi();