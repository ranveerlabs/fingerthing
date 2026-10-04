#ifndef FINGERTHING_ENROLL_H
#define FINGERTHING_ENROLL_H

#include <stdbool.h>
#include "r503.h"

int enroll(r503 *s, bool (*tick)(void));

#endif
