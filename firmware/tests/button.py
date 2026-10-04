from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
src = (root / ".build/pico-fido2/pico-keys-sdk/src/main.c").read_text()
a = src.index("bool wait_button() {")
b = src.index("\n__attribute__((weak)) int picokey_init()", a)
finger_a = src.index("bool wait_fingerprint(bool pin) {")
finger_b = src.index("\n}\n#endif", finger_a) + 2
code = """
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "tusb.h"
#define MODE_BUTTON 2
bool cancel_button;
static bool req_button_pending, online;
static unsigned mode, step, kind;
static unsigned finger_calls;
static bool expected_pin;
static uint32_t clock_ms;
static uint32_t board_millis(void) { return clock_ms; }
bool tud_ready(void) { return online; }
static uint32_t led_get_mode(void) { return mode; }
static void led_set_mode(uint32_t n) { mode = n; }
static void execute_tasks(void) {
    ++step;
    clock_ms += 1000;
    if (kind == 3 && step == 2) cancel_button = true;
    if (kind == 7 && step == 1) {
        online = false;
        tud_suspend_cb(true);
        online = true;
    }
    if (kind == 8 && step == 1) {
        online = false;
        tud_umount_cb();
        online = true;
    }
    if (kind == 9 && step == 2) {
        online = false;
        tud_suspend_cb(false);
    }
    if (kind == 10 && step == 2) tud_mount_cb();
}
static bool picok_board_button_read(void) {
    if (kind == 1) return step == 1;
    if (kind == 2) return step < 2;
    if (kind == 4) return true;
    if (kind == 5) return step < 2 || step == 3;
    if (kind == 3) return step == 1;
    if (kind >= 7) return step == 1;
    return false;
}
static bool finger_wait(bool pin) {
    ++finger_calls;
    assert(pin == expected_pin);
    if (kind == 7) {
        online = false;
        tud_suspend_cb(true);
        online = true;
    }
    if (kind == 8) {
        online = false;
        tud_umount_cb();
        online = true;
    }
    if (kind == 9) online = false;
    if (kind == 10) tud_mount_cb();
    return false;
}
""" + src[a:b] + src[finger_a:finger_b] + """
int main(void) {
    for (unsigned wrap = 0; wrap < 2; ++wrap) {
        for (kind = 0; kind < 11; ++kind) {
            step = 0;
            mode = 7;
            online = kind != 6;
            clock_ms = wrap ? UINT32_MAX - 2000 : 0;
            bool failed = wait_button();
            assert(failed == (kind != 1 && kind != 5));
            assert(!req_button_pending && mode == 7);
            if (kind == 0 || kind == 2 || kind == 4) assert(step == 30);
            if (kind == 3) assert(step == 2);
            if (kind == 6) assert(step == 0);
            if (kind == 7 || kind == 8) assert(step == 1);
            if (kind == 9) assert(step == 2);
            if (kind == 10) assert(step == 2);
        }
    }
    for (unsigned pin = 0; pin < 2; ++pin) {
        for (kind = 1; kind < 11; ++kind) {
            if (kind > 1 && kind < 6) continue;
            online = kind != 6;
            mode = 7;
            finger_calls = 0;
            expected_pin = !!pin;
            assert(wait_fingerprint(!!pin) == (kind != 1));
            assert(finger_calls == (kind != 6));
            assert(!req_button_pending && mode == 7);
        }
    }
    puts("button: press/release, pre-held, timeout, USB cancellation and clock wrap passed");
}
"""
with tempfile.TemporaryDirectory() as tmp:
    c = Path(tmp) / "button.c"
    out = Path(tmp) / "button"
    c.write_text(code)
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-Wall", "-Wextra",
                    "-Werror", "-fsanitize=undefined", "-I" + str(root / "firmware/tests/include"),
                    str(c), str(root / "firmware/src/bus.c"), "-o", str(out)], check=True)
    subprocess.run([str(out)], check=True)
