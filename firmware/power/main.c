#include "pico/stdlib.h"
#include "pico/low_power.h"
#include "hardware/pll.h"
#include "tusb.h"
#include <stdio.h>

static volatile bool suspended;

void tud_mount_cb(void) { suspended = false; }
void tud_umount_cb(void) { suspended = true; }
void tud_resume_cb(void) { suspended = false; }
void tud_suspend_cb(bool wake) {
    (void)wake;
    suspended = true;
}

int main(void) {
    gpio_init(8);
    gpio_put(8, false);
    gpio_set_dir(8, GPIO_OUT);
    for (unsigned pin = 4; pin <= 7; ++pin) {
        if (pin == 6) continue;
        gpio_init(pin);
        gpio_disable_pulls(pin);
    }
    stdio_init_all();
    uint32_t full = clock_get_hz(clk_sys);
    bool slow = false;
    clock_dest_bitset_t keep = clock_dest_bitset_none();
    clock_dest_bitset_add(&keep, CLK_DEST_SYS_TIMER0);
    clock_dest_bitset_add(&keep, CLK_DEST_REF_TICKS);
    for (;;) {
        uint32_t irq = save_and_disable_interrupts();
        bool sleep = suspended;
        gpio_put(8, !sleep && tud_ready());
        if (sleep != slow) {
            if (sleep) {
                uint32_t ref = clock_get_hz(clk_ref);
                clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, ref);
                clock_configure_undivided(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, ref);
                pll_deinit(pll_sys);
            } else {
                set_sys_clock_khz(full / 1000, true);
            }
            slow = sleep;
        }
        if (sleep) {
            low_power_sleep_until_irq(&keep);
        }
        restore_interrupts(irq);
        if (!sleep) {
            if (getchar_timeout_us(0) == '?') printf("sys %lu\n", (unsigned long)clock_get_hz(clk_sys));
            sleep_ms(1);
        }
    }
}
