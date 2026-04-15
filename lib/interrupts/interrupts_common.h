#pragma once

extern volatile uint16_t menu_btn_clicks_count;

void menu_btn_irq();
void back_btn_irq();
void select_btn_irq();
void movefd_btn_irq();
void movebd_btn_irq();