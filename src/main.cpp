#include "config.h"
#include "uart.h"
#include "i2c.h"
#include "lcd.h"
#include "spi.h"
#include "rc522.h"
#include "servo.h"
#include "sonar.h"
#include "cards.h"
#include "display.h"

#include <util/delay.h>

int main(void) {
    servo_init();
    sonar_init();
    i2c_init();
    lcd_init();
    spi_init();
    rc522_init();
    uart_init();

    uart_send_string("=== Sistem parcare pornit ===");
    uart_newline();
    uint8_t version = rc522_read(0x37);
    uart_send_string("RC522 versiune: 0x");
    uart_send_hex(version);
    uart_newline();

    lcd_set_cursor(0, 0);
    lcd_write_string("Sistem parcare");
    lcd_set_cursor(0, 1);
    lcd_write_string("Initializare...");

    _delay_ms(500);
    rc522_reset_field();
    uint8_t dummy[5];
    for (uint8_t i = 0; i < 5; i++) {
        rc522_read_card(dummy);
        _delay_ms(100);
    }

    _delay_ms(1500);

    uint8_t locuri_libere = TOTAL_LOCURI;
    afiseaza_idle(locuri_libere);

    uart_send_string("Stare initiala: idle, locuri = ");
    uart_send_num(locuri_libere);
    uart_send_char('/');
    uart_send_num(TOTAL_LOCURI);
    uart_newline();
    uart_send_string("Astept card...");
    uart_newline();

    while (1) {
        uint8_t uid[5];

        if (rc522_read_card(uid) != 0) {
            _delay_ms(100);
            continue;
        }

        uart_newline();
        uart_send_string(">> Card scanat: UID = ");
        uart_send_uid(uid);
        uart_newline();

        uint8_t card_ok = card_authorized(uid);

        if (!card_ok) {
            uart_send_string("   Status: RESPINS (neautorizat)");
            uart_newline();

            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_write_string("Card respins");
            lcd_set_cursor(0, 1);
            lcd_write_string("Acces interzis");
            _delay_ms(2500);

            rc522_reset_field();

            afiseaza_idle(locuri_libere);
            continue;
        }

        uart_send_string("   Status: AUTORIZAT");
        uart_newline();

        if (locuri_libere == 0) {
            uart_send_string("   PARCARE PLINA - acces refuzat");
            uart_newline();

            afiseaza_parcare_plina();
            _delay_ms(3000);

            rc522_reset_field();
            afiseaza_idle(locuri_libere);
            continue;
        }

        uart_send_string("   Locuri inainte: ");
        uart_send_num(locuri_libere);
        uart_send_char('/');
        uart_send_num(TOTAL_LOCURI);
        uart_newline();
        uart_send_string("   Bariera: SUS");
        uart_newline();

        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_write_string("Card OK!");
        lcd_set_cursor(0, 1);
        lcd_write_string("Bariera: SUS");
        servo_set_position(SERVO_DESCHIS);
        _delay_ms(1000);

        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_write_string("Va rugam treceti");
        lcd_set_cursor(0, 1);
        lcd_write_string("Bariera: SUS");

        uart_send_string("   Astept masina la HC-SR04...");
        uart_newline();

        uint8_t masina_a_trecut = 0;
        uint8_t citiri_consecutive = 0;
        uint16_t cicluri = 300;

        for (uint16_t i = 0; i < cicluri; i++) {
            uint16_t dist = sonar_read_distance_cm();
            if (dist < 30) {
                citiri_consecutive++;
                if (citiri_consecutive >= 3) {
                    masina_a_trecut = 1;
                    uart_send_string("   Masina detectata sub senzor");
                    uart_newline();
                    break;
                }
            } else {
                citiri_consecutive = 0;
            }
            _delay_ms(100);
        }

        if (masina_a_trecut) {
            uint8_t libere_consecutive = 0;
            for (uint16_t i = 0; i < cicluri; i++) {
                uint16_t dist = sonar_read_distance_cm();
                if (dist >= 30) {
                    libere_consecutive++;
                    if (libere_consecutive >= 5) {
                        uart_send_string("   Masina a iesit din zona");
                        uart_newline();
                        break;
                    }
                } else {
                    libere_consecutive = 0;
                }
                _delay_ms(100);
            }
        }

        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_write_string("Bariera: JOS");
        lcd_set_cursor(0, 1);
        if (masina_a_trecut) {
            lcd_write_string("Loc ocupat");
            uart_send_string("   Bariera: JOS - loc ocupat");
            uart_newline();
        } else {
            lcd_write_string("Timeout!");
            uart_send_string("   TIMEOUT - masina nu a aparut");
            uart_newline();
        }
        servo_set_position(SERVO_INCHIS);
        _delay_ms(1500);

        if (masina_a_trecut && locuri_libere > 0) {
            locuri_libere--;
            uart_send_string("   Locuri ramase: ");
            uart_send_num(locuri_libere);
            uart_send_char('/');
            uart_send_num(TOTAL_LOCURI);
            uart_newline();
        }

        uart_send_string("--- Tranzactie incheiata ---");
        uart_newline();

        rc522_reset_field();

        uart_send_string("Astept card...");
        uart_newline();

        afiseaza_idle(locuri_libere);
        _delay_ms(1000);
    }
}
