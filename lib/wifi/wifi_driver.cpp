#include <WiFi.h>
#include <esp_wifi.h>

void init_wifi(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}