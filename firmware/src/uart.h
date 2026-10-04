#ifndef FINGERTHING_UART_H
#define FINGERTHING_UART_H

#include <stdbool.h>
#include "r503.h"

typedef bool (*sensor_tick)(void);
void sensor_init(void);
void sensor_off(void);
int sensor_io(const uint8_t *tx, size_t n, uint8_t *rx, size_t cap, void *ctx);

#endif
