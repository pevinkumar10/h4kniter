#include <WiFi.h>
#include <esp_wifi.h>

void init_wifi(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

void disable_wifi(){
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);
}