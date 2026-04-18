#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include "tool_state.h"
#include "display_common.h"
#include "console_common.h"
#include "features_common.h"

volatile bool is_menu_pressed = false;
volatile bool is_home_pressed = false;
volatile bool is_select_pressed = false;
volatile bool is_movefd_pressed = false;
volatile bool is_movebd_pressed = false;

bool is_sleep_eligible_screen(ToolScreen screen)
{
    return (screen == HOME_SCREEN || screen == MENU_SCREEN);
}

bool has_any_button_activity()
{
    return is_menu_pressed || is_home_pressed || is_select_pressed || is_movefd_pressed || is_movebd_pressed;
}

void enter_idle_sleep()
{
    clear_display();

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