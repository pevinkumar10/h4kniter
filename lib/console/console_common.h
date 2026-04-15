#include "Arduino.h"

#define MENU_BTN_PIN 16
#define BACK_BTN_PIN 17
#define SELECT_BTN_PIN 18
#define MOVEFD_BTN_PIN 19
#define MOVEBD_BTN_PIN 23

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