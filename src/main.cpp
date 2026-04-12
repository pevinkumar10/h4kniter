#include "Arduino.h"

#include "display_common.h"
#include "buttons_common.h"
#include "wifi_common.h"
#include "wifi_scanner.h"

#include "tool_state.h"

volatile bool is_menu_pressed = false;
volatile unsigned long last_interrupt = 0;

ToolScreen current_screen = SLEEP_SCREEN;

void IRAM_ATTR menu_btn_irq(){
    unsigned long now = millis();

    if (now - last_interrupt > 200) {
        is_menu_pressed = true;
        last_interrupt = now;
    }
}

void setup()
{
	Serial.begin(115200);
    init_display();

    setup_button_irq(MENU_BTN_PIN, menu_btn_irq);
}

void loop()
{
    if (is_menu_pressed){
        is_menu_pressed = false;
        current_screen = MENU_SCREEN;
    }

    switch (current_screen){

        case MENU_SCREEN:
            print_text("MENU OPENED",1,1,1,true,true);
            delay(1000);
            current_screen = SLEEP_SCREEN;
            break;

        case SLEEP_SCREEN:
            display.clearDisplay();
            display.display();
            break;
    }

	delay(200);
}