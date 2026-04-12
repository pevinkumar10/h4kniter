#include "Arduino.h"


#include "buttons_common.h"
#include "tool_state.h"

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
    if (is_menu_pressed){
        is_menu_pressed = false;
        current_screen = MENU_SCREEN;
    }

	if (is_home_pressed){
		is_home_pressed = false;
		current_screen = HOME_SCREEN;
	}
}
