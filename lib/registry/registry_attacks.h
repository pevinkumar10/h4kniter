#pragma once

#include "Arduino.h"

struct AttacksList_t {
    int8_t aid;
    const char* attack_tag;
    const char* attack_name;
    const char* attack_category;
    const char* description;
};

struct SubAttacksList_t {
    const char** sub_attacks;
};

extern AttacksList_t WiFiAttacksList[];
extern const int wifi_attack_count;