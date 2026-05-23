#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <avr/io.h>

#define SS_PIN  PB2
#define RST_PIN PD4

void spi_init();
uint8_t spi_transfer(uint8_t data);

#endif
