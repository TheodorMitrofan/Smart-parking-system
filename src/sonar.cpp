#include "config.h"
#include "sonar.h"
#include <avr/io.h>
#include <util/delay.h>

void sonar_init() {
    DDRD |= (1 << PD2);
    DDRD &= ~(1 << PD3);
    PORTD &= ~(1 << PD2);
}

uint16_t sonar_read_distance_cm() {
    PORTD |= (1 << PD2);
    _delay_us(10);
    PORTD &= ~(1 << PD2);

    uint32_t timeout = 30000;
    while (!(PIND & (1 << PD3))) {
        if (--timeout == 0) return 999;
    }

    uint16_t count = 0;
    while (PIND & (1 << PD3)) {
        _delay_us(1);
        count++;
        if (count > 25000) return 999;
    }
    return count / 58;
}
