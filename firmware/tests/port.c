#include "finger.h"
#include "uart.h"
#include <assert.h>
#include <stdio.h>

bool cancel_button;
static bool active, pin, failed;
static int starts, stops;

uint32_t board_millis(void) { return 0; }
bool picok_board_button_read(void) { return false; }
void execute_tasks(void) {}
bool finger_tick(void) { return !cancel_button; }
void sensor_init(void) { assert(!active); active = true; ++starts; }
void sensor_off(void) { assert(active); active = false; ++stops; }
int sensor_io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx) {
    (void)tx; (void)n; (void)rx; (void)cap; (void)ctx;
    return -1;
}
bool finger_gate(r503 *s, bool verified) {
    assert(active && verified == pin && s->io == sensor_io);
    assert(*(sensor_tick *)s->ctx == finger_tick);
    return failed;
}

int main(void) {
    for (int verified = 0; verified < 2; ++verified) {
        pin = verified;
        for (int result = 0; result < 2; ++result) {
            failed = result;
            assert(finger_wait(pin) == failed);
            assert(!active && starts == stops);
        }
    }
    assert(starts == 4);
    puts("port: sensor shutdown after approval and failure, both PIN states passed");
}
