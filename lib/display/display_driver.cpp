#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <display_common.h>

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);

void init_display()  {
    if(!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDR)) { 
        Serial.println("Couldn't init display");
  }
}