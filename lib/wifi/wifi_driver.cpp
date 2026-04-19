#include "WiFi.h"
#include "esp_wifi.h"

void init_wifi(){

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // forcing the initialization to use override sanity_check function
    extern int ieee80211_raw_frame_sanity_check(int32_t, int32_t, int32_t);

    delay(100);
}

void disable_wifi(){
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);
}

void set_promiscuous_mode(){

    esp_wifi_set_promiscuous(true);

}

bool create_ap(const char* ap_name){

    disable_wifi();
    
    WiFi.mode(WIFI_AP);
    
    IPAddress local_IP(192, 168, 10, 1);   // ESP32's own IP (the Gateway)
    IPAddress gateway(192, 168, 10, 1);    // Usually same as local_IP
    IPAddress subnet(255, 255, 255, 0);   // Standard subnet mask

    WiFi.softAPConfig(local_IP, gateway, subnet);
    
    WiFi.softAP(ap_name);

    return true;
}

