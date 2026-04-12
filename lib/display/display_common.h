#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DISPLAY_ADDR 0x3C
#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128

extern Adafruit_SSD1306 display;

void init_display();
void print_text(const char* text, int x, int y, int size);
void show_percentage();
void clear_display();