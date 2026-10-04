#ifndef FINGERTHING_TEST_TUSB_H
#define FINGERTHING_TEST_TUSB_H
#include <stdbool.h>
#include <stdint.h>
bool tud_ready(void);
void tud_mount_cb(void);
void tud_suspend_cb(bool wake);
void tud_umount_cb(void);
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts);
#endif
