import os
from pathlib import Path
import subprocess
import tempfile

src = Path('.build/pico-fido2/pico-keys-sdk/src/fs/low_flash.c').read_text()
start = src.index('#define TOTAL_FLASH_PAGES')
end = src.index('#ifdef PICO_RP2040', start)
prefix = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#define PICO_PLATFORM 1
#define FLASH_SECTOR_SIZE 4096
#define XIP_BASE 0x10000000
#define abort failed
#define printf(...) ((void)0)
typedef int mutex_t;
typedef int semaphore_t;
static jmp_buf stop;
static bool held, irq, acquired;
static int starts, ends, erases, programs, releases, start_fail, end_fail;
static uint32_t erased_size;
_Noreturn static void failed(void) { longjmp(stop, 1); }
static bool mutex_try_enter(mutex_t *m, void *owner) {
    (void)m; (void)owner;
    assert(!held);
    held = true;
    return true;
}
static void mutex_exit(mutex_t *m) { (void)m; assert(held); held = false; }
static void sem_release(semaphore_t *s) { (void)s; ++releases; }
static bool multicore_lockout_start_timeout_us(uint64_t us) {
    assert(us == 1000 && held && !acquired);
    if (++starts <= start_fail) return false;
    acquired = true;
    return true;
}
static bool multicore_lockout_end_timeout_us(uint64_t us) {
    assert(us == 1000 && held && acquired);
    if (++ends <= end_fail) return false;
    acquired = false;
    return true;
}
static uint32_t save_and_disable_interrupts(void) { bool old = irq; irq = true; return old; }
static void restore_interrupts(uint32_t old) { assert(irq); irq = old; }
static void flash_range_erase(uint32_t offset, uint32_t size) {
    assert(held && acquired && irq && offset == 0x80000);
    assert(size == FLASH_SECTOR_SIZE || size == 2 * FLASH_SECTOR_SIZE);
    erased_size = size;
    ++erases;
}
static void flash_range_program(uint32_t offset, const uint8_t *data, size_t size) {
    assert(held && acquired && irq && offset == 0x80000 && data[0] == 42);
    assert(size == FLASH_SECTOR_SIZE && erases == 1);
    ++programs;
}
'''
tests = r'''
int main(void) {
    for (int erase = 0; erase < 2; ++erase) {
        for (int kind = 0; kind < 5; ++kind) {
            memset(flash_pages, 0, sizeof(flash_pages));
            held = acquired = false;
            irq = kind == 4;
            starts = ends = erases = programs = releases = 0;
            start_fail = kind == 1 ? 5 : kind == 2 ? 4 : 0;
            end_fail = kind == 3 ? 5 : 0;
            locked_out = flash_available = true;
            ready_pages = 1;
            flash_pages[0].address = XIP_BASE + 0x80000;
            flash_pages[0].page[0] = 42;
            flash_pages[0].page_size = 2 * FLASH_SECTOR_SIZE;
            flash_pages[0].ready = !erase;
            flash_pages[0].erase = erase;
            int halted = setjmp(stop);
            if (!halted) do_flash();
            assert(irq == (kind == 4));
            assert((halted != 0) == (kind == 1 || kind == 3));
            assert(erases == (kind != 1));
            assert(programs == (!erase && kind != 1));
            if (erases) assert(erased_size == (erase ? 2 : 1) * FLASH_SECTOR_SIZE);
            assert(starts == (kind == 1 || kind == 2 ? 5 : 1));
            assert(ends == (kind == 1 ? 0 : kind == 3 ? 5 : 1));
            if (halted) {
                assert(ready_pages == 1 && held && releases == 0);
                assert(flash_pages[0].ready || flash_pages[0].erase);
            } else {
                assert(ready_pages == 0 && !held && !acquired && releases == 1);
                assert(!flash_pages[0].ready && !flash_pages[0].erase);
            }
        }
    }
    puts("flash: failed lockout halts, bounded retries, IRQ masking and pending pages passed");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    path = Path(tmp)
    (path / 'flash.c').write_text(prefix + src[start:end] + tests)
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c11', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', str(path / 'flash.c'), '-o', str(path / 'flash')], check=True)
    subprocess.run([str(path / 'flash')], check=True,
                   env={**os.environ, 'ASAN_OPTIONS': os.environ.get('ASAN_OPTIONS', 'detect_leaks=0')})
