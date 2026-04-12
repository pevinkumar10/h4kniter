#include "Arduino.h"

#define MENU_BTN_PIN 16
#define BACK_BTN_PIN 17
// #define uint8_t MENU_BTN_PIN 16

void init_buttons();
void update_btns_status();
void setup_button_irq(uint8_t pin, void (*callback)());