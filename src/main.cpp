#include "Arduino.h"

#include "tool_state.h"
#include "display_common.h"
#include "buttons_common.h"
#include "wifi_common.h"

volatile bool is_menu_pressed = false;
volatile bool is_home_pressed = false;

volatile unsigned long last_interrupt = 0;

ToolScreen current_screen = SLEEP_SCREEN;

void IRAM_ATTR menu_btn_irq(){
    unsigned long now = millis();

    if (now - last_interrupt > 200) {
        is_menu_pressed = true;
        last_interrupt = now;
    }
}

void IRAM_ATTR back_btn_irq(){
    unsigned long now = millis();

    if (now - last_interrupt > 200) {
        is_home_pressed = true;
        last_interrupt = now;
    }
}

void check_btn_status(){
    if (is_menu_pressed){
        is_menu_pressed = false;
        current_screen = MENU_SCREEN;
    }

	if (is_home_pressed){
		is_home_pressed = false;
		current_screen = HOME_SCREEN;
	}
}

void setup()
{
	Serial.begin(115200);
    init_display();

    setup_button_irq(MENU_BTN_PIN, menu_btn_irq);
	setup_button_irq(BACK_BTN_PIN, back_btn_irq);
}

void loop()
{
	check_btn_status();

    switch (current_screen){

        case MENU_SCREEN:
            print_text("MENU",1,1,1,true,true);
            break;

        case HOME_SCREEN:
			print_text("HOME",1,1,1,true,true);
            break;
    }

	delay(100);
}