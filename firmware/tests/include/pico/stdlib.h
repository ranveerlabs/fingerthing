#ifndef TEST_STDLIB_H
#define TEST_STDLIB_H
#include <stdint.h>
uint32_t get_absolute_time(void);
static inline uint32_t to_ms_since_boot(uint32_t t) { return t; }
void gpio_set_function(unsigned pin, int fn);
#define GPIO_FUNC_UART 2
#endif
