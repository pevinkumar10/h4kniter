
#pragma once

#include "Arduino.h"

enum ToolScreen {
    HOME_SCREEN,
    MENU_SCREEN,
    SELECT_SCREEN,
    RESULT_SCREEN,
    DEFAULT_SCREEN
};

// global flags (declarations)
extern volatile bool is_menu_pressed;
extern volatile bool is_home_pressed;
extern volatile bool is_select_pressed;

extern int selected_index;
extern const char* selected_tag;

extern ToolScreen current_screen;