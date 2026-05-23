#include "config.h"
#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

static void lcd_send_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0xF0) | LCD_BACKLIGHT | mode;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | LCD_EN);
    _delay_us(1);
    i2c_write(data & ~LCD_EN);
    _delay_us(50);
    i2c_stop();
}

static void lcd_send_byte(uint8_t b, uint8_t m) {
    lcd_send_nibble(b & 0xF0, m);
    lcd_send_nibble((b << 4) & 0xF0, m);
}

void lcd_command(uint8_t c) {
    lcd_send_byte(c, 0);
    _delay_ms(2);
}

void lcd_write_char(char c) {
    lcd_send_byte(c, LCD_RS);
}

void lcd_write_string(const char* s) {
    while (*s) lcd_write_char(*s++);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    lcd_command((row == 0 ? 0x80 : 0xC0) + col);
}

void lcd_clear() {
    lcd_command(0x01);
    _delay_ms(5);
}

void lcd_write_num(uint8_t num) {
    if (num >= 10) lcd_write_char('0' + (num / 10));
    lcd_write_char('0' + (num % 10));
}

void lcd_init() {
    _delay_ms(50);
    lcd_send_nibble(0x30, 0); _delay_ms(5);
    lcd_send_nibble(0x30, 0); _delay_us(150);
    lcd_send_nibble(0x30, 0); _delay_us(150);
    lcd_send_nibble(0x20, 0); _delay_us(150);
    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_clear();
}
