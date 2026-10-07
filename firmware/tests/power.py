import os
from pathlib import Path
import subprocess
import tempfile

src = Path('firmware/power/main.c').read_text()
src = src[src.index('static volatile bool suspended;'):]
prefix = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#define main power_main
#define GPIO_OUT true
#define clk_sys 0
#define clk_ref 1
#define clk_peri 2
#define CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF 1
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS 2
#define CLK_DEST_SYS_TIMER0 1
#define CLK_DEST_REF_TICKS 2
typedef unsigned clock_dest_bitset_t;
static jmp_buf done;
static unsigned kind, saves, sleeps, sys, peri;
static bool irq, pll, powered, ready;
static int pll_sys;
void tud_mount_cb(void);
void tud_umount_cb(void);
void tud_suspend_cb(bool wake);
void tud_resume_cb(void);
static void gpio_init(unsigned pin) { assert(pin == 8 || pin == 4 || pin == 5 || pin == 7); }
static void gpio_disable_pulls(unsigned pin) { assert(pin == 4 || pin == 5 || pin == 7); }
static void gpio_set_dir(unsigned pin, bool out) { assert(pin == 8 && out && !powered); }
static void gpio_put(unsigned pin, bool value) { assert(pin == 8); powered = value; }
static bool stdio_init_all(void) {
    if (kind == 1 || kind == 4) tud_suspend_cb(false);
    if (kind == 2 || kind == 5) { ready = false; tud_umount_cb(); }
    return true;
}
static bool tud_ready(void) { return ready; }
static uint32_t clock_get_hz(unsigned clock) { return clock == clk_ref ? 12000000 : sys; }
static clock_dest_bitset_t clock_dest_bitset_none(void) { return 0; }
static void clock_dest_bitset_add(clock_dest_bitset_t *bits, unsigned bit) { *bits |= bit; }
static void clock_configure_undivided(unsigned clock, unsigned source, unsigned aux, unsigned hz) {
    assert(irq && !powered && hz == 12000000);
    if (clock == clk_sys) { assert(source == 1 && aux == 0); sys = hz; }
    else { assert(clock == clk_peri && source == 0 && aux == 2); peri = hz; }
}
static void pll_deinit(int p) { assert(p == pll_sys && irq && sys == 12000000 && peri == sys); pll = false; }
static bool set_sys_clock_khz(unsigned khz, bool required) {
    assert(irq && khz == 150000 && required);
    sys = peri = khz * 1000;
    pll = true;
    return true;
}
static uint32_t save_and_disable_interrupts(void) {
    bool old = irq;
    if (++saves == 1) {
        if (kind == 0) tud_suspend_cb(false);
        if (kind == 4) tud_resume_cb();
        if (kind == 5) { ready = true; tud_mount_cb(); }
    }
    irq = true;
    return old;
}
static void low_power_sleep_until_irq(const clock_dest_bitset_t *keep) {
    assert(irq && !powered && !pll && sys == 12000000 && peri == sys);
    assert(*keep == (CLK_DEST_SYS_TIMER0 | CLK_DEST_REF_TICKS));
    ++sleeps;
    if (kind == 2) { ready = true; tud_mount_cb(); }
    else tud_resume_cb();
}
static void restore_interrupts(uint32_t old) {
    assert(irq && old == 0);
    irq = false;
    if (kind >= 4 || (sleeps && sys == 150000000)) {
        assert(pll && powered && ready && sys == 150000000);
        assert(sleeps == (kind >= 4 ? 0 : 1));
        longjmp(done, 1);
    }
    if (kind == 3 && saves == 1) tud_suspend_cb(false);
    assert(saves < 4);
}
static int getchar_timeout_us(uint32_t timeout) { assert(timeout == 0); return -1; }
static void sleep_ms(uint32_t delay) { assert(delay == 1 && !irq); }
'''
tests = r'''
#undef main
int main(void) {
    for (kind = 0; kind < 6; ++kind) {
        suspended = irq = powered = false;
        ready = pll = true;
        sys = peri = 150000000;
        saves = sleeps = 0;
        if (!setjmp(done)) power_main();
    }
    puts("power: suspend at IRQ masking, clock reduction, sensor off, resume and reconnect passed");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    path = Path(tmp)
    (path / 'power.c').write_text(prefix + src + tests)
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c11', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', str(path / 'power.c'), '-o', str(path / 'power')], check=True)
    subprocess.run([str(path / 'power')], check=True,
                   env={**os.environ, 'ASAN_OPTIONS': os.environ.get('ASAN_OPTIONS', 'detect_leaks=0')})
