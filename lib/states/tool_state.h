#pragma once

enum ToolScreen {
    HOME_SCREEN,
    MENU_SCREEN,
    SLEEP_SCREEN,
};

// global flags (declarations)
extern volatile bool is_menu_pressed;
extern volatile bool is_home_pressed;
extern ToolScreen current_screen;