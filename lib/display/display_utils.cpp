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

void draw_centered_text(const char* text, int y, int size, bool render, bool clear) {
    int16_t x1, y1;
    uint16_t w, h;
    
    if (clear){
        display.clearDisplay();
    }

    display.setTextSize(size);
    display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);

    int x = (DISPLAY_WIDTH - w) / 2;

    display.setCursor(x, y);
    display.print(text);

    if (render){
        display.display();
    }
}