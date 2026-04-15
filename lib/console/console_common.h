#include "Arduino.h"

#define MENU_BTN_PIN 16
#define BACK_BTN_PIN 17
#define SELECT_BTN_PIN 18
#define MOVEFD_BTN_PIN 19
#define MOVEBD_BTN_PIN 23


enum AttackConfigMask : uint8_t
{
    CFG_OPT_ONE = (1 << 0),
    CFG_OPT_TWO = (1 << 1),
    CFG_OPT_THREE = (1 << 2),
    CFG_OPT_FOUR = (1 << 3)
};

struct ConfigOption
{
    const char *label;
    uint8_t flag;
    bool selected;
};

// Buttons functions
void init_buttons();
void clear_button_state();
void update_btns_status();
void setup_button_irq(uint8_t pin, void (*callback)());

// menu functions
void handle_menu();
void handle_menu_selection();
void handle_result_screen();
void tool_menu_next();
void tool_menu_prev();