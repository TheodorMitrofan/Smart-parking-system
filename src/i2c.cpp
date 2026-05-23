#include "config.h"
#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>

void i2c_init() {
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1 << TWEN);
}

void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(10);
}

void i2c_write(uint8_t d) {
    TWDR = d;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}
