#include "time.h"
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

void draw_h4kniter_home()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Outer border (clean frame)
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

    // Main Title (split for style)
    display.setTextSize(2);
    display.setCursor(18, 18);
    display.print("H4KNITER");

    display.drawLine(0, 85, 128, 45, SSD1306_WHITE);

    print_text("10/08",19,39,1,false);

    print_text("v1",110,52,false);

    display.display();
}