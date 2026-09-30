from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
src = (root / ".build/pico-fido2/pico-keys-sdk/src/main.c").read_text()
a = src.index("bool wait_button() {")
b = src.index("\n__attribute__((weak)) int picokey_init()", a)
code = """
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define MODE_BUTTON 2
static bool cancel_button, req_button_pending;
static unsigned mode, step, kind;
static uint32_t clock_ms;
static uint32_t board_millis(void) { return clock_ms; }
static uint32_t led_get_mode(void) { return mode; }
static void led_set_mode(uint32_t n) { mode = n; }
static void execute_tasks(void) {
    ++step;
    clock_ms += 1000;
    if (kind == 3 && step == 2) cancel_button = true;
}
static bool picok_board_button_read(void) {
    if (kind == 1) return step == 1;
    if (kind == 2) return step < 2;
    if (kind == 4) return true;
    if (kind == 5) return step < 2 || step == 3;
    if (kind == 3) return step == 1;
    return false;
}
""" + src[a:b] + """
int main(void) {
    for (unsigned wrap = 0; wrap < 2; ++wrap) {
        for (kind = 0; kind < 6; ++kind) {
            step = 0;
            mode = 7;
            clock_ms = wrap ? UINT32_MAX - 2000 : 0;
            bool failed = wait_button();
            assert(failed == (kind != 1 && kind != 5));
            assert(!req_button_pending && mode == 7);
            if (kind == 0 || kind == 2 || kind == 4) assert(step == 30);
            if (kind == 3) assert(step == 2);
        }
    }
    puts("button: press/release, pre-held, timeout, cancel and clock wrap passed");
}
"""
with tempfile.TemporaryDirectory() as tmp:
    c = Path(tmp) / "button.c"
    out = Path(tmp) / "button"
    c.write_text(code)
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-Wall", "-Wextra",
                    "-Werror", "-fsanitize=undefined", str(c), "-o", str(out)], check=True)
    subprocess.run([str(out)], check=True)
