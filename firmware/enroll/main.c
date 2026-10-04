#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "enroll.h"
#include "uart.h"

static uint32_t start;

static bool tick(void) {
    return stdio_usb_connected() && getchar_timeout_us(0) != 27 &&
        (uint32_t)(to_ms_since_boot(get_absolute_time()) - start) < 60000;
}

int main(void) {
    stdio_init_all();
    sensor_init();
    sensor_tick poll = tick;
    r503 s = {sensor_io, &poll};
    for (;;) {
        while (!stdio_usb_connected()) sleep_ms(100);
        puts("e: replace fingerprint in slot 0");
        int ch = getchar_timeout_us(60000000);
        if (ch != 'e') continue;
        while (getchar_timeout_us(0) >= 0) {}
        puts("replace slot 0? y");
        if (getchar_timeout_us(10000000) != 'y') continue;
        start = to_ms_since_boot(get_absolute_time());
        int ret = enroll(&s, tick);
        puts(ret ? "failed" : "saved");
        puts("flash the sensor firmware before use");
    }
}
