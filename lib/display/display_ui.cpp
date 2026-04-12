#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "display_common.h"

void clear_display(){
    display.clearDisplay();
    display.display();
}

void print_text(const char* text, int x, int y,int size, bool render, bool clear){

    if (clear){
        display.clearDisplay();
    }
    
    display.setTextSize(size);      
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(x,y);

    display.println(text);
    
    if (render){
        display.display();
    }
}

void draw_centered_text(const char* text, int y, int size) {
    int16_t x1, y1;
    uint16_t w, h;

    display.setTextSize(size);
    display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);

    int x = (DISPLAY_WIDTH - w) / 2;

    display.setCursor(x, y);
    display.print(text);
}

void draw_progress_bar(int progress, int y) {
    int bar_width = 100;
    int bar_height = 10;

    int x = (DISPLAY_WIDTH - bar_width) / 2;  // center horizontally

    // Draw border
    display.drawRect(x, y, bar_width, bar_height, SSD1306_WHITE);

    // Fill based on progress (0–100)
    int fill_width = (progress * (bar_width - 2)) / 100;

    display.fillRect(x + 1, y + 1, fill_width, bar_height - 2, SSD1306_WHITE);
}

void show_loading_bar(const char* text, int size, int delay_sec, bool show_persentage){

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    for (int i = 0; i <= 100; i++) {

        display.clearDisplay();

        // 🔹 Centered title
        draw_centered_text(text, 10, size);

        // 🔹 Progress bar
        draw_progress_bar(i, 35);

        if (show_persentage){
            char buf[10];
            snprintf(buf, sizeof(buf), "%d%%", i);
            draw_centered_text(buf, 50, 1);
        }

        display.display();

        delay(delay_sec);  // control speed
    }

}