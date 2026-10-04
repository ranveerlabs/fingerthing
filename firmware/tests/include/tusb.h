#ifndef FINGERTHING_TEST_TUSB_H
#define FINGERTHING_TEST_TUSB_H
#include <stdbool.h>
bool tud_ready(void);
void tud_mount_cb(void);
void tud_suspend_cb(bool wake);
void tud_umount_cb(void);
#endif
