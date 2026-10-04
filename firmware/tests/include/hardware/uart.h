#ifndef TEST_UART_H
#define TEST_UART_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define uart1 1
#define UART_PARITY_NONE 0
void uart_init(int uart, unsigned baud);
void uart_set_hw_flow(int uart, bool cts, bool rts);
void uart_set_format(int uart, unsigned bits, unsigned stop, int parity);
bool uart_is_readable(int uart);
char uart_getc(int uart);
void uart_write_blocking(int uart, const uint8_t *p, size_t n);
#endif
