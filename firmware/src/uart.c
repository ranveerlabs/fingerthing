#include "uart.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

static bool powered, ready;
static uint32_t start;

void sensor_off(void) {
    if (ready) uart_deinit(uart1);
    ready = powered = false;
    for (unsigned pin = 4; pin <= 5; ++pin) {
        gpio_init(pin);
        gpio_disable_pulls(pin);
    }
#ifdef FINGERTHING_SENSOR_POWER_PIN
    gpio_init(FINGERTHING_SENSOR_POWER_PIN);
    gpio_disable_pulls(FINGERTHING_SENSOR_POWER_PIN);
    gpio_put(FINGERTHING_SENSOR_POWER_PIN, false);
    gpio_set_dir(FINGERTHING_SENSOR_POWER_PIN, GPIO_OUT);
#endif
}

void sensor_init(void) {
    sensor_off();
#ifdef FINGERTHING_SENSOR_POWER_PIN
    gpio_put(FINGERTHING_SENSOR_POWER_PIN, true);
#endif
    start = to_ms_since_boot(get_absolute_time());
    powered = true;
}

static bool sensor_ready(sensor_tick tick) {
    if (!powered) return false;
    if (ready) return true;
    while ((uint32_t)(to_ms_since_boot(get_absolute_time()) - start) < 250) {
        if (!tick()) return false;
    }
    if (!tick()) return false;
    uart_init(uart1, 57600);
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);
    uart_set_hw_flow(uart1, false, false);
    uart_set_format(uart1, 8, 1, UART_PARITY_NONE);
    ready = true;
    return true;
}

int sensor_io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx) {
    if (!ctx || !tx || !n || n > 27 || !rx || cap < 12) return -1;
    sensor_tick tick = *(sensor_tick *)ctx;
    if (!tick || !sensor_ready(tick)) return -1;
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
