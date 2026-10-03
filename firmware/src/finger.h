#ifndef FINGERTHING_FINGER_H
#define FINGERTHING_FINGER_H

#include <stdbool.h>
#include <stdint.h>
#include "r503.h"

bool wait_fingerprint(bool pin);
bool finger_wait(bool pin);
bool finger_gate(r503 *s, bool pin);
bool finger_tick(void);
uint32_t finger_now(void);
bool finger_down(void);
bool finger_cancel(void);
void finger_poll(void);

#endif
