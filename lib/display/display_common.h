#pragma once

#define DISPLAY_ADDR 0x3C
#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

extern Adafruit_SSD1306 display;

void init_display();
void clear_display();
void display_home();
void display_menu();
void menu_next();

void draw_centered_text(const char* text, int y, int size,bool render = true, bool clear = false);
void print_text(const char* text, int x, int y,int size, bool render = true, bool clear = false);
void show_loading_bar(const char* text, int size, int delay_sec,bool show_persentage = false);
