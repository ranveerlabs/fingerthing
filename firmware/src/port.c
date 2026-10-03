#include "finger.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "bsp/board.h"

extern bool cancel_button;
extern bool picok_board_button_read(void);
extern void execute_tasks(void);

uint32_t finger_now(void) { return board_millis(); }
bool finger_down(void) { return picok_board_button_read(); }
bool finger_cancel(void) { return cancel_button; }
void finger_poll(void) { execute_tasks(); }

static int io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx) {
    (void)ctx;
    for (unsigned i = 0; i < 128 && uart_is_readable(uart1); ++i) uart_getc(uart1);
    if (uart_is_readable(uart1) || !finger_tick()) return -1;
    uart_write_blocking(uart1, tx, n);
    uint32_t start = finger_now();
    size_t got = 0, need = 9;
    while (got < need) {
        if (!finger_tick() || (uint32_t)(finger_now() - start) >= 1000) return -1;
        if (!uart_is_readable(uart1)) continue;
        rx[got++] = (uint8_t)uart_getc(uart1);
        if (got == 9) {
            need += ((size_t)rx[7] << 8) | rx[8];
            if (need < 12 || need > cap) return -1;
        }
    }
    return (int)got;
}

bool finger_wait(bool pin) {
    static bool init;
    if (!init) {
        uart_init(uart1, 57600);
        gpio_set_function(4, GPIO_FUNC_UART);
        gpio_set_function(5, GPIO_FUNC_UART);
        uart_set_hw_flow(uart1, false, false);
        uart_set_format(uart1, 8, 1, UART_PARITY_NONE);
        init = true;
    }
    r503 s = {io, NULL};
    return finger_gate(&s, pin);
}
