#include "tool_state.h"
#include "buttons_common.h"
#include "interrupts_common.h"
#include "display_common.h"

volatile unsigned long menu_btn_last_interrupt = 0;
volatile unsigned long home_btn_last_interrupt = 0;

void IRAM_ATTR menu_btn_irq(){
    unsigned long now = millis();

    if (now - menu_btn_last_interrupt > 500) {
        is_menu_pressed = true;
        menu_btn_last_interrupt = now;
        menu_next();
    }
}

void IRAM_ATTR back_btn_irq(){
    unsigned long now = millis();

    if (now - home_btn_last_interrupt > 500) {
        is_home_pressed = true;
        home_btn_last_interrupt = now;
    }
}