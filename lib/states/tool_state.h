#pragma once

enum ToolScreen {
    LOADING_SCREEN,
    MENU_SCREEN,
    SLEEP_SCREEN
};

// global flags (declarations)
extern volatile bool is_menu_pressed;
extern volatile bool is_scanning;
extern ToolScreen current_screen;