#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "enroll.h"
#include "uart.h"
#include "tusb.h"

static uint32_t start;
static volatile bool cancelled;

void tud_mount_cb(void) { cancelled = true; }
void tud_umount_cb(void) { cancelled = true; }
void tud_suspend_cb(bool wake) {
    (void)wake;
    cancelled = true;
}
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    (void)itf;
    (void)rts;
    if (!dtr) cancelled = true;
}

static bool tick(void) {
    return stdio_usb_connected() && getchar_timeout_us(0) != 27 &&
        (uint32_t)(to_ms_since_boot(get_absolute_time()) - start) < 60000 && !cancelled;
}

int main(void) {
    stdio_init_all();
    sensor_off();
    sensor_tick poll = tick;
    r503 s = {sensor_io, &poll};
    for (;;) {
        while (!stdio_usb_connected()) sleep_ms(100);
        cancelled = false;
        puts("e: replace fingerprint in slot 0");
        int ch = getchar_timeout_us(60000000);
        if (ch != 'e' || cancelled || !stdio_usb_connected()) continue;
        int queued = 0;
        for (unsigned i = 0; i < 128 && !cancelled; ++i) {
            queued = getchar_timeout_us(0);
            if (queued < 0) break;
        }
        if (queued >= 0 || cancelled || !stdio_usb_connected()) continue;
        puts("replace slot 0? y");
        if (getchar_timeout_us(10000000) != 'y' || cancelled || !stdio_usb_connected()) continue;
        start = to_ms_since_boot(get_absolute_time());
        sensor_init();
        int ret = enroll(&s, tick);
        sensor_off();
        puts(ret ? "failed" : "saved");
        puts("flash the sensor firmware before use");
    }
}
