#include "config.h"
#include "uart.h"
#include <avr/io.h>

void uart_init() {
    UBRR0H = (UBRR_VAL >> 8);
    UBRR0L = UBRR_VAL;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* s) {
    while (*s) uart_send_char(*s++);
}

void uart_send_num(uint8_t num) {
    if (num >= 100) {
        uart_send_char('0' + (num / 100));
        uart_send_char('0' + ((num / 10) % 10));
        uart_send_char('0' + (num % 10));
    } else if (num >= 10) {
        uart_send_char('0' + (num / 10));
        uart_send_char('0' + (num % 10));
    } else {
        uart_send_char('0' + num);
    }
}

void uart_send_hex(uint8_t b) {
    const char hex[] = "0123456789ABCDEF";
    uart_send_char(hex[(b >> 4) & 0x0F]);
    uart_send_char(hex[b & 0x0F]);
}

void uart_send_uid(const uint8_t* uid) {
    for (uint8_t i = 0; i < 4; i++) {
        uart_send_hex(uid[i]);
        if (i < 3) uart_send_char(' ');
    }
}

void uart_newline() {
    uart_send_char('\r');
    uart_send_char('\n');
}
