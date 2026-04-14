#include "Arduino.h"

#include "console_common.h"
#include "display_common.h"
#include "tool_state.h"
#include "registry_attacks.h"

void init_buttons(){
    pinMode(MENU_BTN_PIN,INPUT_PULLDOWN);
}

void setup_button_irq(uint8_t pin, void (*callback)()){
    
    pinMode(pin, INPUT_PULLDOWN);

    attachInterrupt(
        digitalPinToInterrupt(pin),
        callback,
        RISING
    );
}

void update_btns_status(){
    if (current_screen == RESULT_SCREEN){
        // RESULT_SCREEN handles button actions in main loop.
        return;
    }

    if (is_home_pressed){
        is_home_pressed = false;
        is_menu_pressed = false;
        is_select_pressed = false;

        // Back/Home navigation hierarchy.
        if (current_screen == RESULT_SCREEN){
            current_screen = MENU_SCREEN;
        } else if (current_screen == MENU_SCREEN){
            current_screen = HOME_SCREEN;
        } else {
            current_screen = HOME_SCREEN;
        }

        clear_display();
    }

    if (is_menu_pressed){
        is_menu_pressed = false;

        if (current_screen == MENU_SCREEN){
            tool_menu_next();
        } else if (current_screen == RESULT_SCREEN) {
            // Menu is disabled on result screen (back only).
        } else {
            // Always start from first item when opening menu.
            selected_index = 0;
            current_screen = MENU_SCREEN;
            clear_display();
        }
    }

    if (is_select_pressed){
        is_select_pressed = false;

        if (current_screen == MENU_SCREEN){
            selected_tag = WiFiAttacksList[selected_index].attack_tag;
            current_screen = SELECT_SCREEN;
            clear_display();
        }
        // Select is ignored on non-menu screens.
    }
}
