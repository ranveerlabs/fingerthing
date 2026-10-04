from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
src = (root / ".build/pico-fido2/pico-keys-sdk/src/main.c").read_text()
a = src.index("bool picok_board_button_read(void) {\n#ifdef FINGERTHING_BUTTON_PIN")
b = src.index("\n#else\nbool picok_board_button_read(void)", a)
code = """
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define FINGERTHING_BUTTON_PIN 6
#define GPIO_IN 0
static uint32_t now, base;
static bool high;
static unsigned initialized, direction, pull;
static uint32_t board_millis(void) { return now; }
static void gpio_init(unsigned pin) { assert(pin == 6); ++initialized; }
static void gpio_set_dir(unsigned pin, bool out) {
    assert(pin == 6 && !out);
    ++direction;
}
static void gpio_pull_up(unsigned pin) { assert(pin == 6); ++pull; }
static bool gpio_get(unsigned pin) { assert(pin == 6); return high; }
""" + src[a:b] + """
static void check(unsigned ms, bool level, bool down) {
    now = base + ms;
    high = level;
    assert(picok_board_button_read() == down);
    assert(initialized == 1 && direction == 1 && pull == 1);
}
int main(int argc, char **argv) {
    (void)argv;
    base = argc > 1 ? UINT32_MAX - 15 : 0;
    check(0, false, true);
    check(1, true, true);
    check(10, false, true);
    check(11, true, true);
    check(30, true, true);
    check(31, true, false);
    check(40, false, false);
    check(49, true, false);
    check(50, false, false);
    check(69, false, false);
    check(70, false, true);
    check(80, true, true);
    check(99, true, true);
    check(100, true, false);
    puts("gpio: active low, held start, bounce, stable edges and clock wrap passed");
}
"""
with tempfile.TemporaryDirectory() as tmp:
    c = Path(tmp) / "gpio.c"
    out = Path(tmp) / "gpio"
    c.write_text(code)
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-Wall", "-Wextra",
                    "-Werror", "-fsanitize=undefined", str(c), "-o", str(out)], check=True)
    subprocess.run([str(out)], check=True)
    subprocess.run([str(out), "wrap"], check=True)
