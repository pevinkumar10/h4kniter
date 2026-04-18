#include "Arduino.h"

#include "overrides.h"
#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "interrupts_common.h"
#include "wifi_common.h"
#include "features_common.h"

ToolScreen current_screen = HOME_SCREEN;
ResultType scan_result_type = NO_R;

void setup()
{
    init_display();

    Serial.begin(115200);
    
    show_rect_zoom("BOOTING",1,15);
    setup_button_irq(MENU_BTN_PIN, menu_btn_irq);
    setup_button_irq(BACK_BTN_PIN, back_btn_irq);
    setup_button_irq(SELECT_BTN_PIN, select_btn_irq);
    setup_button_irq(MOVEFD_BTN_PIN, movefd_btn_irq);
    setup_button_irq(MOVEBD_BTN_PIN, movebd_btn_irq);

    clear_display();
}

void loop()
{
    static unsigned long last_action_at = millis();
    static ToolScreen last_screen = current_screen;
    ToolScreen loop_start_screen = current_screen;

    if (current_screen != last_screen)
    {
        last_screen = current_screen;
        last_action_at = millis();
    }

    if (is_sleep_eligible_screen(current_screen) && has_any_button_activity())
    {
        last_action_at = millis();
    }

    update_btns_status();

    switch (current_screen)
    {

    case MENU_SCREEN:
        handle_attacks_menu();
        break;

    case HOME_SCREEN:
        handle_home();
        break;

    case SELECT_SCREEN:
        handle_menu_selection();
        break;

    case RESULT_SCREEN:
        handle_result_screen();
        break;
    }

    // Handle transitions that happened inside handlers (including long-running flows).
    if (current_screen != loop_start_screen)
    {
        last_action_at = millis();
        last_screen = current_screen;
    }

    if (is_sleep_eligible_screen(current_screen) && (millis() - last_action_at >= NO_ACTION_SLEEP_MS))
    {
        enter_idle_sleep();
        last_action_at = millis();
        last_screen = current_screen;
    }

    delay(150);
}