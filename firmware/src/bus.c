#include "tusb.h"

extern bool cancel_button;

void tud_mount_cb(void) {
    cancel_button = true;
}

void tud_suspend_cb(bool wake) {
    (void)wake;
    cancel_button = true;
}

void tud_umount_cb(void) {
    cancel_button = true;
}
