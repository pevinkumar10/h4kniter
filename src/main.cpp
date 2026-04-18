#include "Arduino.h"

#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "interrupts_common.h"
#include "wifi_common.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    return 0;
}

static const unsigned long NO_ACTION_SLEEP_MS = 30000UL;

static bool is_sleep_eligible_screen(ToolScreen screen)
{
    return (screen == HOME_SCREEN || screen == MENU_SCREEN);
}

static bool has_any_button_activity()
{
    return is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed;
}

static void enter_idle_sleep()
{
    clear_display();

    // Deep sleep on ESP32 can only wake from RTC-capable GPIOs.
    // If current button pins are not RTC-capable, fall back to light sleep
    // so "any button wakes device" still works with existing wiring.
    const int button_pins[] = {
        MENU_BTN_PIN,
        BACK_BTN_PIN,
        SELECT_BTN_PIN,
        MOVEFD_BTN_PIN,
        MOVEBD_BTN_PIN
    };

    uint64_t rtc_wakeup_mask = 0;
    for (size_t i = 0; i < (sizeof(button_pins) / sizeof(button_pins[0])); i++)
    {
        if (rtc_gpio_is_valid_gpio((gpio_num_t)button_pins[i]))
        {
            rtc_wakeup_mask |= (1ULL << button_pins[i]);
        }
    }

    if (rtc_wakeup_mask != 0)
    {
        esp_sleep_enable_ext1_wakeup(rtc_wakeup_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
        esp_deep_sleep_start();
        return;
    }

    for (size_t i = 0; i < (sizeof(button_pins) / sizeof(button_pins[0])); i++)
    {
        gpio_wakeup_enable((gpio_num_t)button_pins[i], GPIO_INTR_HIGH_LEVEL);
    }
    esp_sleep_enable_gpio_wakeup();
    esp_light_sleep_start();

    for (size_t i = 0; i < (sizeof(button_pins) / sizeof(button_pins[0])); i++)
    {
        gpio_wakeup_disable((gpio_num_t)button_pins[i]);
    }

    clear_button_state();
}

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
    // show_loading_bar("BOOTING",1,10);
    
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