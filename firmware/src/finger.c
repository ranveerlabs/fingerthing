#include "finger.h"

static uint32_t start;
static bool fallback, ready, pressed, approved;

bool finger_tick(void) {
    finger_poll();
    if (finger_cancel() || (uint32_t)(finger_now() - start) >= 30000) return false;
    bool down = finger_down();
    if (!down) {
        if (fallback && pressed) approved = true;
        ready = true;
    } else if (ready) {
        pressed = true;
    }
    return !approved;
}

bool finger_gate(r503 *s, bool pin) {
    start = finger_now();
    fallback = pin;
    ready = !finger_down();
    pressed = approved = false;
    bool live = r503_init(s) == 0;
    bool clear = false;
    uint32_t last = finger_now() - 100;
    while (finger_tick()) {
        if (!live || (uint32_t)(finger_now() - last) < 100) continue;
        last = finger_now();
        int ret = r503_image(s);
        if (!finger_tick()) break;
        if (ret == 2) {
            clear = true;
        } else if (ret == 0 && clear) {
            ret = r503_make(s, 1);
            if (!finger_tick()) break;
            if (!ret) ret = r503_match(s);
            if (!finger_tick()) break;
            if (!ret) return false;
            live = false;
        } else if (ret != 0) {
            live = false;
        }
    }
    return !approved || finger_cancel() || (uint32_t)(finger_now() - start) >= 30000;
}
