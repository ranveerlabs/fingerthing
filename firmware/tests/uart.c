#include "uart.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint32_t now, base;
static int kind, pos, writes;
static const uint8_t ack[] = {0xef,1,255,255,255,255,7,0,3,0,0,10};

uint32_t get_absolute_time(void) { return now; }
void gpio_set_function(unsigned pin, int fn) { assert((pin == 4 || pin == 5) && fn == 2); }
void uart_init(int uart, unsigned baud) { assert(uart == 1 && baud == 57600); }
void uart_set_hw_flow(int uart, bool cts, bool rts) { assert(uart == 1 && !cts && !rts); }
void uart_set_format(int uart, unsigned bits, unsigned stop, int parity) {
    assert(uart == 1 && bits == 8 && stop == 1 && parity == 0);
}
bool uart_is_readable(int uart) {
    assert(uart == 1);
    if (!writes) return kind == 1;
    if (kind == 2 || kind == 6) return false;
    if (kind == 5 && (uint32_t)(now - base) < 500) return false;
    return pos < (int)sizeof(ack);
}
char uart_getc(int uart) {
    assert(uart_is_readable(uart));
    if (!writes) return 0;
    uint8_t b = ack[pos++];
    if (kind == 4 && pos == 9) b = 255;
    return (char)b;
}
void uart_write_blocking(int uart, const uint8_t *p, size_t n) {
    assert(uart == 1 && n == 12 && p[9] == 1);
    ++writes;
}
static bool poll(void) {
    now += 2;
    return kind != 3;
}

int main(void) {
    sensor_init();
    sensor_tick tick = poll;
    r503 s = {sensor_io, &tick};
    for (int wrap = 0; wrap < 2; ++wrap) {
        for (kind = 0; kind <= 6; ++kind) {
            now = base = wrap ? UINT32_MAX - 100 : 0;
            writes = pos = 0;
            assert((r503_image(&s) == 0) == (kind == 0 || kind == 5));
            if (kind == 1 || kind == 3) assert(writes == 0);
            assert((uint32_t)(now - base) <= 1002);
        }
    }
    uint8_t tx[12] = {0}, rx[12];
    assert(sensor_io(tx, sizeof(tx), rx, 8, &tick) == -1);
    assert(sensor_io(tx, sizeof(tx), rx, sizeof(rx), NULL) == -1);
    puts("uart: flood, cancellation, bad lengths, delayed replies and timeout passed");
}
