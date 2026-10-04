#ifndef TEST_STDLIB_H
#define TEST_STDLIB_H
#include <stdint.h>
#include <stdbool.h>
uint32_t get_absolute_time(void);
static inline uint32_t to_ms_since_boot(uint32_t t) { return t; }
void gpio_set_function(unsigned pin, int fn);
void gpio_init(unsigned pin);
void gpio_disable_pulls(unsigned pin);
void gpio_put(unsigned pin, bool value);
void gpio_set_dir(unsigned pin, bool out);
#define GPIO_FUNC_UART 2
#define GPIO_OUT true
#endif
