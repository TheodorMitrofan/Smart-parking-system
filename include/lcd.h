#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#define LCD_BACKLIGHT 0x08
#define LCD_EN        0x04
#define LCD_RS        0x01

void lcd_init();
void lcd_clear();
void lcd_command(uint8_t c);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_write_char(char c);
void lcd_write_string(const char* s);
void lcd_write_num(uint8_t num);

#endif
