#include <assert.h>
#include <setjmp.h>
#define main enrollment_main
#include "../enroll/main.c"
#undef main

static jmp_buf done;
static uint32_t now;
static int mode, prompts, reads, confirms, attempts, stopped, drained;
static bool active;

bool stdio_init_all(void) { return true; }
bool stdio_usb_connected(void) { return true; }
uint32_t get_absolute_time(void) { return now; }
void sleep_ms(uint32_t delay) { now += delay; }
void sensor_init(void) { assert(!active); active = true; ++attempts; }
void sensor_off(void) { if (active) ++stopped; active = false; }
int sensor_io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx) {
    (void)tx; (void)n; (void)rx; (void)cap; (void)ctx;
    return -1;
}
int getchar_timeout_us(uint32_t timeout) {
    if (timeout == 60000000) {
        if (++prompts == 3) longjmp(done, 1);
        if (prompts == 1 && mode == 1) tud_suspend_cb(false);
        return 'e';
    }
    if (timeout == 10000000) {
        ++confirms;
        if (prompts == 1 && mode == 2) tud_mount_cb();
        return 'y';
    }
    assert(timeout == 0);
    if (!active && prompts == 1 && mode == 7) {
        assert(++drained <= 128);
        return '.';
    }
    if (active) {
        ++reads;
        if (prompts == 1 && mode == 6) return 27;
    }
    return -1;
}
int enroll(r503 *s, bool (*poll)(void)) {
    assert(active && s->io == sensor_io && poll == tick);
    if (!poll()) {
        assert(prompts == 1 && mode == 6);
        return -1;
    }
    if (prompts == 1) {
        if (mode == 3) tud_umount_cb();
        if (mode == 4) tud_suspend_cb(true);
        if (mode == 5) now += 60000;
        if (mode == 8) {
            tud_cdc_line_state_cb(0, false, false);
            tud_cdc_line_state_cb(0, true, false);
        }
    }
    bool live = poll();
    assert(live == (prompts == 2 || mode == 0));
    return live ? 0 : -1;
}

int main(void) {
    for (mode = 0; mode <= 8; ++mode) {
        now = UINT32_MAX - 100;
        prompts = reads = confirms = attempts = stopped = drained = 0;
        active = false;
        if (!setjmp(done)) enrollment_main();
        assert(!active && attempts == stopped);
        assert(attempts == (mode == 1 || mode == 2 || mode == 7 ? 1 : 2));
        assert(confirms == (mode == 1 || mode == 7 ? 1 : 2));
    }
    puts("enrollment USB: prompt cancellation, flood, reconnect, suspend, timeout wrap and fresh retry passed");
}
