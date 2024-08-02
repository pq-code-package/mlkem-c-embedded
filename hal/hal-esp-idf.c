// SPDX-License-Identifier: Apache-2.0
#include <hal.h>
#include <hal/systimer_hal.h>
#include <hal/wdt_hal.h>
#include <hal/cache_ll.h>
#include <hal/cache_hal.h>
#include <soc/soc.h>
#include <esp_private/systimer.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

extern int main();

/*NOTE: refer to esp-idf/components/bootloader/subproject/main/ld/esp32c3/bootloader.ld*/
extern int _bss_start;
extern int _bss_end;
extern int bootloader_usable_dram_end; /* start of the stack */
static char *stack_start = &bootloader_usable_dram_end;
static systimer_hal_context_t hal_ctx;

extern int uart_tx_one_char(uint8_t c);
void hal_setup(const enum clock_mode clock) {
    systimer_hal_init(&hal_ctx);
    systimer_hal_tick_rate_ops_t ops = {
        .ticks_to_us = systimer_ticks_to_us,
        .us_to_ticks = systimer_us_to_ticks,
    };

    systimer_hal_set_tick_rate_ops(&hal_ctx, &ops);
    systimer_hal_enable_counter(&hal_ctx, 0);
};

void hal_send_str(const char *in) {
    const char *cur = in;
    while (*cur) {
        uart_tx_one_char(*cur);
        cur += 1;
    }
    uart_tx_one_char('\n');
}


/* Reference: what have done in `esp_timer_impl_get_time` in esp_timer/src/esp_timer_impl_systimer.c` */
uint64_t hal_get_time(void) {
    return systimer_hal_get_time(&hal_ctx, 0);
}

size_t hal_get_stack_size(void) {
    register char *cur_stack;
    __asm__ volatile ("mv %0, sp" : "=r"(cur_stack));
    return cur_stack - stack_start;

}

void hal_spraystack(void) {}
size_t hal_checkstack(void) { return 0; }

void __attribute__((noreturn)) call_start_cpu0(void) {
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_set_flashboot_en(&wdt0_context, false);

    wdt_hal_context_t rwdt_context = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rwdt_context);
    wdt_hal_disable(&rwdt_context);
    wdt_hal_set_flashboot_en(&rwdt_context, false);

    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, RTC_CNTL_SWD_WKEY_VALUE);
    REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_AUTO_FEED_EN);
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, 0);

    cache_hal_init();
    cache_hal_is_cache_enabled(CACHE_LL_ID_ALL, CACHE_TYPE_ALL);

    hal_setup(CLOCK_BENCHMARK);

    char buf[1024];
    snprintf(buf, 1024, "stack start: %X\n", (int)&bootloader_usable_dram_end);
    hal_send_str(buf);

    (void) main();

    for (;;) (void) 0;
}


#include <errno.h>
#undef errno
extern int errno;

int __wrap__open(char *file, int flags, int mode) {
    (void) file;
    (void) flags;
    (void) mode;
    errno = ENOSYS;
    return -1;
}

int __wrap__close(int fd) {
    errno = ENOSYS;
    (void) fd;
    return -1;
}

#include <sys/stat.h>

int __wrap__fstat(int fd, struct stat *buf) {
    (void) fd;
    (void) buf;
    errno = ENOSYS;
    return -1;
}

int __wrap__getpid(void) {
    errno = ENOSYS;
    return -1;
}

int __wrap__isatty(int file) {
    (void) file;
    errno = ENOSYS;
    return 0;
}

int __wrap__kill(int pid, int sig) {
    (void) pid;
    (void) sig;
    errno = ENOSYS;
    return -1;
}

int __wrap__lseek(int fd, int ptr, int dir) {
    (void) fd;
    (void) ptr;
    (void) dir;
    errno = ENOSYS;
    return -1;
}

int __wrap__read(int fd, char *ptr, int len) {
    (void) fd;
    (void) ptr;
    (void) len;
    errno = ENOSYS;
    return -1;
}

int __wrap__write(int fd, const char *ptr, int len) {
    (void) fd;
    (void) ptr;
    (void) len;
    errno = ENOSYS;
    return -1;
}

void *__wrap__sbrk (int incr) {
    char *prev_heap_end;
    prev_heap_end = stack_start;
    stack_start += incr;

    return (void *) prev_heap_end;
}

struct _reent * __getreent(void)
{
    return _impure_ptr;
}

