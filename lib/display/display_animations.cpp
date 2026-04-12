#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "display_common.h"
#include "wifi_attacks.h"

int selected_index = 0;

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

void display_home(){

    draw_centered_text("H4KNITER",10,1,true,true);
    //print_text("H4KNITER",,32,1,true,true);
}

void update_menu(){
    
}

void menu_next()
{
    const int menu_size = sizeof(WifiAttacksList) / sizeof(WifiAttacksList[0]);

    selected_index++;

    if (selected_index >= menu_size)
    {
        selected_index = 0;
    }
}

void display_menu()
{
    const int items_per_page = 5;
    const int menu_size = sizeof(WifiAttacksList) / sizeof(WifiAttacksList[0]);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    int page = selected_index / items_per_page;
    int start = page * items_per_page;
    int end = start + items_per_page;

    if (end > menu_size) end = menu_size;

    int y = 0;

    for (int i = start; i < end; i++)
    {
        if (i == selected_index)
        {
            display.fillRect(0, y, 128, 10, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        }
        else
        {
            display.setTextColor(SSD1306_WHITE);
        }

        display.setCursor(2, y);
        display.print(WifiAttacksList[i]);

        y += 12;
    }

    display.display();
}