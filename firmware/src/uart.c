#include "uart.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

void sensor_init(void) {
    uart_init(uart1, 57600);
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);
    uart_set_hw_flow(uart1, false, false);
    uart_set_format(uart1, 8, 1, UART_PARITY_NONE);
}

int sensor_io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx) {
    if (!ctx || !tx || !n || n > 27 || !rx || cap < 12) return -1;
    sensor_tick tick = *(sensor_tick *)ctx;
    if (!tick) return -1;
    for (unsigned i = 0; i < 128 && uart_is_readable(uart1); ++i) uart_getc(uart1);
    if (uart_is_readable(uart1) || !tick()) return -1;
    uart_write_blocking(uart1, tx, n);
    uint32_t start = to_ms_since_boot(get_absolute_time());
    size_t got = 0, need = 9;
    while (got < need) {
        if (!tick() || (uint32_t)(to_ms_since_boot(get_absolute_time()) - start) >= 1000) return -1;
        if (!uart_is_readable(uart1)) continue;
        rx[got++] = (uint8_t)uart_getc(uart1);
        if (got == 9) {
            need += ((size_t)rx[7] << 8) | rx[8];
            if (need < 12 || need > cap) return -1;
        }
    }
    return (int)got;
}
