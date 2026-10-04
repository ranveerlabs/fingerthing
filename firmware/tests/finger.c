#include "finger.h"
#include <assert.h>
#include <stdio.h>
#include "tusb.h"

static uint32_t now, base;
static int mode, images, matches;
bool cancel_button;

uint32_t finger_now(void) { return now; }
void finger_poll(void) {
    now += 10;
    if (mode == 8 && (uint32_t)(now - base) >= 200) tud_suspend_cb(true);
    if (mode == 9 && (uint32_t)(now - base) >= 100) tud_umount_cb();
}
bool finger_cancel(void) { return cancel_button || (mode == 6 && (uint32_t)(now - base) >= 200); }
bool finger_down(void) {
    uint32_t t = now - base;
    if (mode == 3) return t >= 200 && t < 400;
    if (mode == 8) return t >= 100 && t < 400;
    if (mode == 4) return t < 400;
    return false;
}
int r503_init(r503 *s) { (void)s; return mode == 3 || mode == 5 ? -1 : 0; }
int r503_image(r503 *s) {
    (void)s;
    ++images;
    if (mode == 0 || mode == 6 || mode == 8) return 2;
    if (mode == 4) return 0;
    return images == 1 ? 2 : 0;
}
int r503_make(r503 *s, uint8_t slot) {
    (void)s;
    assert(slot == 1);
    if (mode == 7) now += 30000;
    return 0;
}
int r503_match(r503 *s) {
    (void)s;
    ++matches;
    if (mode == 10) tud_suspend_cb(false);
    if (mode == 11) tud_mount_cb();
    return mode == 2 ? 9 : 0;
}

static void run(int kind, bool pin, bool fail, uint32_t clock) {
    mode = kind;
    now = base = clock;
    images = matches = 0;
    cancel_button = false;
    r503 s = {0};
    assert(finger_gate(&s, pin) == fail);
    if (kind == 4) assert(matches == 0);
    if (kind == 2) assert(matches == 1);
    if (kind == 6 || kind >= 8) assert((uint32_t)(now - base) < 1000);
    if (fail && kind != 6 && kind < 8) assert((uint32_t)(now - base) >= 30000);
}

int main(void) {
    for (int i = 0; i < 2; ++i) {
        uint32_t start = i ? UINT32_MAX - 100 : 0;
        run(0, false, true, start);
        run(1, false, false, start);
        run(2, false, true, start);
        run(3, true, false, start);
        run(3, false, true, start);
        run(7, false, true, start);
        run(4, true, true, start);
        run(5, true, true, start);
        run(6, true, true, start);
        run(8, true, true, start);
        run(9, true, true, start);
        run(10, true, true, start);
        run(11, true, true, start);
    }
    puts("finger: fresh touch, mismatch, held input, USB cancellation and timer wrap passed");
}
