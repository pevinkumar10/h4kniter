#include "Arduino.h"

#define MENU_BTN_PIN 19
// #define uint8_t MENU_BTN_PIN 16

void init_buttons();
void read_button(uint8_t button, bool &status);
void setup_button_irq(uint8_t pin, void (*callback)());