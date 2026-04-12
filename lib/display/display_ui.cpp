#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <display_common.h>

void print_text(const char* text, int x, int y,int size){
    display.clearDisplay();
    
    display.setTextSize(size);      
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(x,y);

    display.println(F(text));
    
    display.display();
}

void show_percentage(){
    for (int i = 0; i <= 100; i++) {
        display.clearDisplay();

        // Draw percentage text
        char buf[10];
        sprintf(buf, "%d%%", i);
        
        print_text(buf, 62, 32, 1);

        display.display();
        delay(50);
    }
}

void clear_display(){
    display.clearDisplay();
    display.display();
}