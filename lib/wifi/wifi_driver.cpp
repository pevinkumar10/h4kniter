#include <WiFi.h>
#include "esp_wifi.h"

void init_wifi(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
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