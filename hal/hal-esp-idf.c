// SPDX-License-Identifier: Apache-2.0
#include <hal.h>
#include <hal/systimer_hal.h>
#include <hal/wdt_hal.h>
#include <hal/cache_ll.h>
#include <hal/cache_hal.h>
#include <hal/clk_tree_ll.h>
#include <esp_cpu.h>
#include <soc/soc.h>
#include <soc/clk_tree_defs.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

extern int main();

/*NOTE: refer to esp-idf/components/bootloader/subproject/main/ld/esp32c3/bootloader.ld*/
extern char _stack_bottom; /* start of the stack */
static char *stack_start = &_stack_bottom;
static systimer_hal_context_t hal_ctx;

extern int uart_tx_one_char(uint8_t c);

void hal_setup(const enum clock_mode clock) {
    // disable rtc watch dog to avoid chip reset
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_set_flashboot_en(&wdt0_context, false);

    wdt_hal_context_t rwdt_context = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rwdt_context);
    wdt_hal_disable(&rwdt_context);
    wdt_hal_set_flashboot_en(&rwdt_context, false);

    // disable super watch dog to avoid chip reset
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, RTC_CNTL_SWD_WKEY_VALUE);
    REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_AUTO_FEED_EN);
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, 0);

    cache_hal_init();
    cache_hal_is_cache_enabled(CACHE_LL_ID_ALL, CACHE_TYPE_ALL);

    // setup for cycle count
    systimer_hal_init(&hal_ctx);
    systimer_hal_enable_counter(&hal_ctx, 0);

    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL);
    switch (clock) {
    case CLOCK_BENCHMARK:
        // TODO: need to check with the datasheet
        // max clock frequency for flash
        clk_ll_cpu_set_freq_mhz_from_pll(CLK_LL_PLL_80M_FREQ_MHZ);
        break;
    case CLOCK_FAST:
    default:
        clk_ll_cpu_set_freq_mhz_from_pll(CLK_LL_PLL_160M_FREQ_MHZ);
        break;
    }
};

void hal_send_str(const char *in) {
    const char *cur = in;
    while (*cur) {
        uart_tx_one_char(*cur);
        cur += 1;
    }
    uart_tx_one_char('\n');
}


uint64_t hal_get_time(void) {
    return esp_cpu_get_cycle_count();
}

size_t hal_get_stack_size(void) {
    register char *cur_stack;
    __asm__ volatile ("mv %0, sp" : "=r"(cur_stack));
    return cur_stack - stack_start;
}

void hal_spraystack(void) {}
size_t hal_checkstack(void) {
    return 0;
}

void _exit(int) {
    for (;;) {
        (void) 0;
    }
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

struct _reent *__getreent(void) {
    return _impure_ptr;
}
