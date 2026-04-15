#pragma once

#define DISPLAY_ADDR 0x3C
#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

enum ResultType{
    WIFI_SCAN_R,
    EVIL_TWIN_R,
    NO_R
};

extern Adafruit_SSD1306 display;
extern ResultType scan_result_type;

void init_display();
void clear_display();
void handle_home();

void draw_centered_text(const char* text, int y, int size,bool render = true, bool clear = false);
void print_text(const char* text, int x, int y,int size, bool render = true, bool clear = false);
void show_loading_bar(const char* text, int size, int delay_sec,bool show_persentage = false);
void show_task_progress_frame(const char* text, uint8_t percent, uint8_t phase, bool show_percent = true);
