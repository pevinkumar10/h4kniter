#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "display_common.h"
#include "registry_attacks.h"

void _draw_progress_bar(int progress, int y) {
    int bar_width = 100;
    int bar_height = 10;

    int x = (DISPLAY_WIDTH - bar_width) / 2;  // center horizontally

    // Draw border
    display.drawRect(x, y, bar_width, bar_height, SSD1306_WHITE);

    // Fill based on progress (0–100)
    int fill_width = (progress * (bar_width - 2)) / 100;

    display.fillRect(x + 1, y + 1, fill_width, bar_height - 2, SSD1306_WHITE);
}

void show_task_progress_frame(const char* text, uint8_t percent, uint8_t phase, bool show_percent){
    if (percent > 100) percent = 100;

    const int bar_x = 14;
    const int bar_y = 34;
    const int bar_w = 100;
    const int bar_h = 10;
    const int dot_y = 50;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    // Customizable title text for any task.
    draw_centered_text(text, 10, 1, false, false);

    display.drawRect(bar_x, bar_y, bar_w, bar_h, SSD1306_WHITE);
    int fill_width = (percent * (bar_w - 2)) / 100;
    display.fillRect(bar_x + 1, bar_y + 1, fill_width, bar_h - 2, SSD1306_WHITE);

    // Indeterminate activity dots so user sees active work while waiting.
    for (uint8_t i = 0; i < 3; i++) {
        int x = 52 + (i * 12);
        if (((phase + i) % 3) == 0) {
            display.fillCircle(x, dot_y, 2, SSD1306_WHITE);
        } else {
            display.drawCircle(x, dot_y, 2, SSD1306_WHITE);
        }
    }

    if (show_percent){
        char buf[10];
        snprintf(buf, sizeof(buf), "%u%%", percent);
        draw_centered_text(buf, 56, 1, false, false);
    }

    display.display();
}

void show_loading_bar(const char* text, int size, int delay_sec, bool show_persentage){

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    for (int i = 0; i <= 100; i++) {

        // Centered title
        draw_centered_text(text, 10, size,true);

        // Progress bar
        _draw_progress_bar(i, 35);

        if (show_persentage){
            char buf[10];
            snprintf(buf, sizeof(buf), "%d%%", i);
            draw_centered_text(buf, 50, 1,true);
        }

        display.display();

        delay(delay_sec);  // control speed
    }
}

void handle_home(){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("H4KNITER");
    display.setTextSize(1);
    display.setCursor(90, 30);
    display.print("v1");
    display.display();

}
