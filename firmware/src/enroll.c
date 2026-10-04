#include "enroll.h"
#include <stdio.h>

static int image(r503 *s, bool (*tick)(void), bool down) {
    while (tick()) {
        int ret = r503_image(s);
        if (!tick()) return -1;
        if (ret != 0 && ret != 2) return ret;
        if ((ret == 0) == down) return 0;
    }
    return -1;
}

int enroll(r503 *s, bool (*tick)(void)) {
    if (!tick || !tick()) return -1;
    int ret = r503_init(s);
    if (ret || !tick()) return ret ? ret : -1;
    for (uint8_t slot = 1; slot <= 2; ++slot) {
        puts("lift finger");
        ret = image(s, tick, false);
        if (ret) return ret;
        puts("touch finger");
        ret = image(s, tick, true);
        if (ret) return ret;
        ret = r503_make(s, slot);
        if (ret || !tick()) return ret ? ret : -1;
    }
    ret = r503_store(s);
    if (!ret && !tick()) return -1;
    return ret;
}
