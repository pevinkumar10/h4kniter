#include "Arduino.h"

#define MENU_BTN_PIN 16
#define BACK_BTN_PIN 17
#define SELECT_BTN_PIN 18

// Buttons functions
void init_buttons();
void update_btns_status();
void setup_button_irq(uint8_t pin, void (*callback)());

// menu functions
void handle_menu();
void handle_selection();
void tool_menu_next();