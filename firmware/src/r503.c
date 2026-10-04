#include "r503.h"
#include <string.h>

static uint16_t sum(const uint8_t *p, size_t n) {
    uint16_t v = 0;
    for (size_t i = 0; i < n; ++i) v += p[i];
    return v;
}

static uint16_t word(const uint8_t *p) {
    return ((uint16_t)p[0] << 8) | p[1];
}

int r503_cmd(r503 *s, const uint8_t *cmd, size_t n, uint8_t *out, size_t len) {
    if (!s || !s->io || !cmd || !n || n > 16 || len > 32 || (len && !out)) return -1;
    if (out) memset(out, 0, len);
    uint8_t tx[27] = {0xef, 1, 0xff, 0xff, 0xff, 0xff, 1, 0, 0};
    uint8_t rx[46] = {0};
    tx[8] = (uint8_t)(n + 2);
    memcpy(tx + 9, cmd, n);
    uint16_t check = sum(tx + 6, n + 3);
    tx[n + 9] = (uint8_t)(check >> 8);
    tx[n + 10] = (uint8_t)check;
    int got = s->io(tx, n + 11, rx, sizeof(rx), s->ctx);
    if (got < 12 || got > (int)sizeof(rx)) return -1;
    if (memcmp(rx, tx, 6) || rx[6] != 7) return -1;
    size_t size = word(rx + 7);
    if (size < 3 || size + 9 != (size_t)got) return -1;
    if (sum(rx + 6, size + 1) != word(rx + got - 2)) return -1;
    if (rx[9]) return rx[9];
    if (size != len + 3) return -1;
    if (len) memcpy(out, rx + 10, len);
    return 0;
}

int r503_init(r503 *s) {
    const uint8_t password[] = {0x13, 0, 0, 0, 0};
    const uint8_t level[] = {0x0e, 5, 5};
    int ret = r503_cmd(s, password, sizeof(password), NULL, 0);
    if (ret) return ret;
    const uint8_t read[] = {0x0f};
    uint8_t out[16];
    ret = r503_cmd(s, read, sizeof(read), out, sizeof(out));
    if (ret) return ret;
    if (word(out + 6) != 5) {
        ret = r503_cmd(s, level, sizeof(level), NULL, 0);
        if (ret) return ret;
        ret = r503_cmd(s, read, sizeof(read), out, sizeof(out));
        if (ret) return ret;
    }
    return word(out + 6) == 5 ? 0 : -1;
}

int r503_image(r503 *s) {
    const uint8_t cmd[] = {1};
    return r503_cmd(s, cmd, sizeof(cmd), NULL, 0);
}

int r503_make(r503 *s, uint8_t slot) {
    if (slot < 1 || slot > 2) return -1;
    const uint8_t cmd[] = {2, slot};
    return r503_cmd(s, cmd, sizeof(cmd), NULL, 0);
}

int r503_match(r503 *s) {
    const uint8_t cmd[] = {4, 1, 0, 0, 0, 1};
    uint8_t out[4];
    int ret = r503_cmd(s, cmd, sizeof(cmd), out, sizeof(out));
    if (ret) return ret;
    return word(out) == 0 ? 0 : -1;
}

int r503_store(r503 *s) {
    const uint8_t merge[] = {5};
    const uint8_t save[] = {6, 1, 0, 0};
    int ret = r503_cmd(s, merge, sizeof(merge), NULL, 0);
    if (ret) return ret;
    return r503_cmd(s, save, sizeof(save), NULL, 0);
}
