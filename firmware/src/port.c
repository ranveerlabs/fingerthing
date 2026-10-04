#include "finger.h"
#include "pico/stdlib.h"
#include "uart.h"
#include "bsp/board.h"

extern bool cancel_button;
extern bool picok_board_button_read(void);
extern void execute_tasks(void);

uint32_t finger_now(void) { return board_millis(); }
bool finger_down(void) { return picok_board_button_read(); }
bool finger_cancel(void) { return cancel_button; }
void finger_poll(void) { execute_tasks(); }

bool finger_wait(bool pin) {
    sensor_init();
    sensor_tick tick = finger_tick;
    r503 s = {sensor_io, &tick};
    bool failed = finger_gate(&s, pin);
    sensor_off();
    return failed;
}
