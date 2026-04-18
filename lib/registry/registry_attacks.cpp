#include "registry_attacks.h"

#include "registry_attacks.h"

AttacksList_t WiFiAttacksList[] = {
    {1, "scan_network","Scan Networks", "wifi", "Scan nearby WiFi"},
    {2, "attack_deauth","Deauth", "wifi", "Disconnect clients"},
    {3, "attack_eviltwin","Evil Twin", "wifi", "Create cloned AP for selected clients"},
    {4, "attack_deauth_eviltwin","Deauth & Evil Twin", "wifi", "Perform both Deauth & Evil Twin"}
};

extern AttacksList_t WiFiAttacksList[];

const int wifi_attack_count = sizeof(WiFiAttacksList) / sizeof(WiFiAttacksList[0]);