#include "enroll.h"
#include <assert.h>
#include <stdio.h>

static int kind, calls, images, made, stored;
static const int scans[] = {0,0,2,2,0,0,2,0};

static bool tick(void) {
    ++calls;
    return calls < 100 && !(kind == 5 && made == 1);
}
int r503_init(r503 *s) { (void)s; return kind == 1 ? -1 : 0; }
int r503_image(r503 *s) {
    (void)s;
    if (kind == 2) return 0;
    if (kind == 3 && images == 5) return 7;
    assert(images < (int)(sizeof(scans) / sizeof(scans[0])));
    return scans[images++];
}
int r503_make(r503 *s, uint8_t slot) {
    (void)s;
    assert(slot == ++made);
    return kind == 4 ? 6 : 0;
}
int r503_store(r503 *s) {
    (void)s;
    assert(made == 2 && images == 8);
    ++stored;
    return kind == 6 ? 10 : 0;
}

int main(void) {
    r503 s = {0};
    assert(enroll(&s, NULL) == -1);
    for (kind = 0; kind <= 6; ++kind) {
        calls = images = made = stored = 0;
        int ret = enroll(&s, tick);
        assert((ret == 0) == (kind == 0));
        assert(stored == (kind == 0 || kind == 6));
    }
    puts("enroll: two fresh captures, cancellation and failures before storage passed");
}
