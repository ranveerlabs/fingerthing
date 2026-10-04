#ifndef FINGERTHING_R503_H
#define FINGERTHING_R503_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int (*io)(const uint8_t *, size_t, uint8_t *, size_t, void *);
    void *ctx;
} r503;

int r503_cmd(r503 *s, const uint8_t *cmd, size_t n, uint8_t *out, size_t len);
int r503_init(r503 *s);
int r503_image(r503 *s);
int r503_make(r503 *s, uint8_t slot);
int r503_match(r503 *s);
int r503_store(r503 *s);

#endif
