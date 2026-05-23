#include "config.h"
#include "spi.h"

void spi_init() {
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << SS_PIN);
    DDRB &= ~(1 << PB4);
    PORTB |= (1 << SS_PIN);
    DDRD |= (1 << RST_PIN);
    PORTD |= (1 << RST_PIN);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
}

uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}
