#include "config.h"
#include "display.h"
#include "lcd.h"

void afiseaza_idle(uint8_t locuri_libere) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_write_string("Scanati cartela");
    lcd_set_cursor(0, 1);
    lcd_write_string("Libere: ");
    lcd_write_num(locuri_libere);
    lcd_write_char('/');
    lcd_write_num(TOTAL_LOCURI);
}

void afiseaza_parcare_plina() {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_write_string("PARCARE PLINA!");
    lcd_set_cursor(0, 1);
    lcd_write_string("Reveniti mai tz");
}
