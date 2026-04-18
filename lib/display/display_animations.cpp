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

void show_rect_zoom(const char* text, int cycles, int speed_delay)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    int center_x = 64;
    int center_y = 32;

    for (int c = 0; c < cycles; c++)
    {
        // 🔁 Zoom IN
        for (int size = 60; size > 4; size -= 4)
        {
            display.clearDisplay();

            // Draw concentric triangles
            for (int i = 0; i < 6; i++)
            {
                int s = size - (i * 5);
                if (s > 0)
                {
                    int x = center_x - s / 2;
                    int y = center_y - s / 2;

                    // skewed lines → triangle illusion
                    display.drawLine(x, y + s, x + s/2, y, SSD1306_WHITE);
                    display.drawLine(x + s, y + s, x + s/2, y, SSD1306_WHITE);
                }
            }

            // Optional text
            if (text != NULL)
                draw_centered_text(text, 50, 1, false, false);

            display.display();
            delay(speed_delay);
        }

        // 🔁 Zoom OUT
        for (int size = 4; size < 60; size += 4)
        {
            display.clearDisplay();

            for (int i = 0; i < 6; i++)
            {
                int s = size - (i * 5);
                if (s > 0)
                {
                    int x = center_x - s / 2;
                    int y = center_y - s / 2;

                    // skewed lines → triangle illusion
                    display.drawLine(x, y + s, x + s/2, y, SSD1306_WHITE);
                    display.drawLine(x + s, y + s, x + s/2, y, SSD1306_WHITE);
                }
            }

            if (text != NULL)
                draw_centered_text(text, 50, 1, false, false);

            display.display();
            delay(speed_delay);
        }
    }
}

void show_text_zoom_with_sub(const char* main_text,const char* sub_text,int max_size,int speed_delay, bool render)
{
    int center_y = 20;

    // 🔁 Zoom main text
    for (int size = 1; size <= max_size; size++)
    {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);

        // Main text (centered, scaling)
        draw_centered_text(main_text, center_y - (size * 4), size, false, false);

        if (render){
            display.display();
        }
        
        delay(speed_delay);
    }

    // ✨ Hold + show subtitle gradually
    for (int i = 0; i < 3; i++)
    {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);

        // Final main text (fixed size)
        draw_centered_text(main_text, center_y - (max_size * 4), max_size, false, false);

        // Subtitle (appears with slight delay effect)
        if (i >= 1)
            draw_centered_text(sub_text, 45, 1, false, false);

        if (render){
            display.display();
        }
        
        delay(120);
    }
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
    draw_h4kniter_home();
}
