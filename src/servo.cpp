#include "config.h"
#include "servo.h"
#include <avr/io.h>
#include <util/delay.h>

/* Calibrare CR servo (ajusteaza dupa observatie):
 *   OCR_STOP      = 1.5ms (rotor oprit)
 *   OCR_DIR_OPEN  = 1.2ms (sens deschidere)
 *   OCR_DIR_CLOSE = 1.8ms (sens inchidere)
 *   ROT_TIME_MS   = timpul pentru ~90 grade la viteza data */
#define OCR_STOP        3000
#define OCR_DIR_OPEN    2400
#define OCR_DIR_CLOSE   3600
#define ROT_TIME_MS     300

void servo_init() {
    PORTB &= ~(1 << PB1);
    DDRB  |=  (1 << PB1);

    ICR1   = 40000;
    OCR1A  = OCR_STOP;

    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13)  | (1 << WGM12) | (1 << CS11);
}

void servo_set_position(uint16_t state) {
    if (state == SERVO_DESCHIS) {
        OCR1A = OCR_DIR_OPEN;
    } else {
        OCR1A = OCR_DIR_CLOSE;
    }
    _delay_ms(ROT_TIME_MS);
    OCR1A = OCR_STOP;
}
