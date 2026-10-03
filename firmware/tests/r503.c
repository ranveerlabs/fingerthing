#include "r503.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t reply[46], sent[27];
static int size, calls, init_mode, level;
static void ack(uint8_t status, const uint8_t *data, size_t n);
static size_t sent_size;

static int io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx) {
    (void)ctx;
    assert(n <= sizeof(sent));
    memcpy(sent, tx, n);
    sent_size = n;
    ++calls;
    if (init_mode) {
        if (tx[9] == 0x0f) {
            uint8_t data[16] = {0};
            data[7] = (uint8_t)level;
            ack(0, data, sizeof(data));
        } else if (tx[9] == 0x0e) {
            if (init_mode == 1) level = tx[11];
            ack(0, NULL, 0);
        } else ack(0, NULL, 0);
    }
    if (size > 0 && size <= (int)cap) memcpy(rx, reply, (size_t)size);
    return size;
}

static void ack(uint8_t status, const uint8_t *data, size_t n) {
    memset(reply, 0, sizeof(reply));
    reply[0] = 0xef;
    reply[1] = 1;
    memset(reply + 2, 0xff, 4);
    reply[6] = 7;
    reply[8] = (uint8_t)(n + 3);
    reply[9] = status;
    if (n) memcpy(reply + 10, data, n);
    size = (int)n + 12;
    unsigned sum = 0;
    for (int i = 6; i < size - 2; ++i) sum += reply[i];
    reply[size - 2] = (uint8_t)(sum >> 8);
    reply[size - 1] = (uint8_t)sum;
}

int main(void) {
    r503 s = {io, NULL};
    const uint8_t image[] = {0xef,1,255,255,255,255,1,0,3,1,0,5};
    ack(0, NULL, 0);
    assert(r503_image(&s) == 0);
    assert(sent_size == sizeof(image) && !memcmp(sent, image, sizeof(image)));
    for (int i = 0; i < 12; ++i) {
        ack(0, NULL, 0);
        reply[i] ^= 1;
        assert(r503_image(&s) != 0);
    }
    for (int n = -1; n < 12; ++n) {
        ack(0, NULL, 0);
        size = n;
        assert(r503_image(&s) == -1);
    }
    ack(0, NULL, 0);
    size = 47;
    assert(r503_image(&s) == -1);
    for (int n = 1; n < 256; ++n) {
        ack((uint8_t)n, NULL, 0);
        assert(r503_image(&s) == n);
    }
    const uint8_t match[] = {0,0,0,90};
    ack(0, match, sizeof(match));
    assert(r503_match(&s) == 0);
    const uint8_t search[] = {0xef,1,255,255,255,255,1,0,8,4,1,0,0,0,1,0,15};
    assert(sent_size == sizeof(search) && !memcmp(sent, search, sizeof(search)));
    const uint8_t wrong[] = {0,1,0,90};
    ack(0, wrong, sizeof(wrong));
    assert(r503_match(&s) == -1);
    ack(0, NULL, 0);
    assert(r503_match(&s) == -1);
    ack(9, NULL, 0);
    assert(r503_match(&s) == 9);
    calls = 0;
    ack(10, NULL, 0);
    assert(r503_store(&s) == 10 && calls == 1);
    calls = 0;
    ack(0, NULL, 0);
    assert(r503_store(&s) == 0 && calls == 2);
    assert(sent[9] == 6 && sent[10] == 1 && sent[11] == 0 && sent[12] == 0);
    calls = 0;
    assert(r503_make(&s, 0) == -1 && r503_make(&s, 3) == -1 && calls == 0);
    assert(r503_cmd(NULL, image, 1, NULL, 0) == -1);
    assert(r503_cmd(&s, image, 17, NULL, 0) == -1);
    assert(r503_cmd(&s, image, 1, NULL, 1) == -1);
    init_mode = 1;
    level = 5;
    calls = 0;
    assert(r503_init(&s) == 0 && calls == 2);
    level = 1;
    calls = 0;
    assert(r503_init(&s) == 0 && level == 5 && calls == 4);
    init_mode = 2;
    level = 1;
    assert(r503_init(&s) == -1);
    puts("r503: framing, checksum, truncation, errors, slot bounds and store failure passed");
}
