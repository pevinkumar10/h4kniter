#include "Arduino.h"

#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "interrupts_common.h"
#include "wifi_common.h"

volatile bool is_menu_pressed = false;
volatile bool is_home_pressed = false;
volatile bool is_select_pressed = false;
volatile bool is_movefd_pressed = false;
volatile bool is_movebd_pressed = false;

ToolScreen current_screen = HOME_SCREEN;
ResultType scan_result_type = NO_R;

void setup()
{
    init_display();

    Serial.begin(115200);
    // show_loading_bar("BOOTING",1,5);

    setup_button_irq(MENU_BTN_PIN, menu_btn_irq);
    setup_button_irq(BACK_BTN_PIN, back_btn_irq);
    setup_button_irq(SELECT_BTN_PIN, select_btn_irq);
    setup_button_irq(MOVEFD_BTN_PIN, movefd_btn_irq);
    setup_button_irq(MOVEBD_BTN_PIN, movebd_btn_irq);

    clear_display();
}

void loop()
{
    update_btns_status();

    switch (current_screen)
    {

    case MENU_SCREEN:
        handle_menu();
        break;

    case HOME_SCREEN:
        handle_home();
        break;

    case SELECT_SCREEN:
        handle_menu_selection();
        break;

    case DEFAULT_SCREEN:
        print_text("DEFAULT", 1, 1, 1);
        break;

    case RESULT_SCREEN:
        handle_result_screen();
        break;
    }
    
    delay(150);
}