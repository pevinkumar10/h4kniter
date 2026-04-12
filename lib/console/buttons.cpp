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

void read_button(uint8_t button, bool &status){
	int button_state = digitalRead(button);

	int last_press = 0;

	if (button_state == LOW && millis() - last_press > 200) {
		last_press = millis();
        status = true;
	} else {
        status = false;
    }
}
