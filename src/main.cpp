#include "Arduino.h"

#include "tool_state.h"
#include "display_common.h"
#include "buttons_common.h"
#include "interrupts_common.h"
#include "wifi_common.h"

volatile bool is_menu_pressed = false;
volatile bool is_home_pressed = false;

ToolScreen current_screen = SLEEP_SCREEN;

void setup()
{
    init_display();
    setup_button_irq(MENU_BTN_PIN, menu_btn_irq);
	setup_button_irq(BACK_BTN_PIN, back_btn_irq);
}

void loop()
{
	update_btns_status();

    switch (current_screen){

        case MENU_SCREEN:
            print_text("MENU",1,1,1,true,true);
            break;

        case HOME_SCREEN:
			print_text("HOME",1,1,1,true,true);
            break;
    }

	delay(200);
}