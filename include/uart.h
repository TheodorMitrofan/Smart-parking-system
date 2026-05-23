#ifndef UART_H
#define UART_H

#include <stdint.h>

#define BAUD     9600
#define UBRR_VAL ((F_CPU / 16 / BAUD) - 1)

void uart_init();
void uart_send_char(char c);
void uart_send_string(const char* s);
void uart_send_num(uint8_t num);
void uart_send_hex(uint8_t b);
void uart_send_uid(const uint8_t* uid);
void uart_newline();

#endif
