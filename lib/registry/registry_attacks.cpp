#include "registry_attacks.h"

AttacksList_t WiFiAttacksList[] = {
    {1, "scan_network","Scan Networks", "wifi", "Scan nearby WiFi."},
    {2, "attack_deauth","Deauth", "wifi", "Disconnect clients."},
    {3, "attack_eviltwin","Evil Twin", "wifi", "Create cloned AP for selected clients."}};

AttacksList_t BleAttacksList[]{
    {1, "scan_network","Scan Networks", "ble", "Scan nearby ble devices."},
    {2, "samsung_spam","Samsung Spam", "wifi", "Spam with samsung devices."},
    {3, "airpod_spam","AirPods Spam", "wifi", "Spam with Airpods pair request."}
};

extern AttacksList_t WiFiAttacksList[];
extern AttacksList_t BleAttacksList[];

const int wifi_attack_count = sizeof(WiFiAttacksList) / sizeof(WiFiAttacksList[0]);
const int ble_attack_count = sizeof(BleAttacksList) / sizeof(BleAttacksList[0]);