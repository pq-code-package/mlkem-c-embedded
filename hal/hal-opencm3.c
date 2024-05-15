// SPDX-License-Identifier: Apache-2.0
#include "hal.h"
#include <sys/cdefs.h>

#define SERIAL_BAUD 38400

#include <libopencm3/cm3/dwt.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/flash.h>

#if defined(STM32F207ZG)
#include <libopencm3/stm32/rng.h>

#define SERIAL_GPIO GPIOD
#define SERIAL_USART USART3
#define SERIAL_PINS (GPIO8 | GPIO9)
#define STM32
#define NUCLEO_BOARD
#elif defined(STM32F407VG)
#include <libopencm3/stm32/rng.h>
#define SERIAL_GPIO GPIOA
#define SERIAL_USART USART2
#define SERIAL_PINS (GPIO2 | GPIO3)
#define STM32
#define DISCOVERY_BOARD

/* 24 MHz */
const struct rcc_clock_scale benchmarkclock = {
    .pllm = 8, //VCOin = HSE / PLLM = 1 MHz
    .plln = 192, //VCOout = VCOin * PLLN = 192 MHz
    .pllp = 8, //PLLCLK = VCOout / PLLP = 24 MHz (low to have 0WS)
    .pllq = 4, //PLL48CLK = VCOout / PLLQ = 48 MHz (required for USB, RNG)
    .pllr = 0,
    .hpre = RCC_CFGR_HPRE_DIV_NONE,
    .ppre1 = RCC_CFGR_PPRE_DIV_2,
    .ppre2 = RCC_CFGR_PPRE_DIV_NONE,
    .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
    .voltage_scale = PWR_SCALE1,
    .flash_config = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_LATENCY_0WS,
    .ahb_frequency = 24000000,
    .apb1_frequency = 12000000,
    .apb2_frequency = 24000000,
};

#elif defined(STM32F767ZI)
#include <libopencm3/stm32/rng.h>
#include <libopencm3/stm32/f7/rcc.h>

#define SERIAL_GPIO GPIOD
#define SERIAL_USART USART3
#define SERIAL_PINS (GPIO8 | GPIO9)
#define STM32

#else
#error Unsupported libopencm3 board
#endif

#define _RCC_CAT(A, B) A ## _ ## B
#define RCC_ID(NAME) _RCC_CAT(RCC, NAME)

__attribute__((unused))
static uint32_t _clock_freq;

#ifdef STM32F2
extern uint32_t rcc_apb1_frequency;
extern uint32_t rcc_apb2_frequency;
#endif

static void clock_setup(enum clock_mode clock) {
    #if defined(DISCOVERY_BOARD)
    switch (clock) {
    case CLOCK_BENCHMARK:
        rcc_clock_setup_pll(&benchmarkclock);
        break;
    case CLOCK_FAST:
    default:
        rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
        break;
    }

    rcc_periph_clock_enable(RCC_RNG);

    flash_prefetch_enable();
    #elif defined(STM32F7)
    switch (clock) {
    case CLOCK_BENCHMARK:
        rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_24MHZ]);
        break;
    case CLOCK_FAST:
    default:
        rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_216MHZ]);
        break;
    }

    rcc_periph_clock_enable(RCC_RNG);
    flash_art_enable();
    flash_prefetch_enable();
    #elif defined(STM32F2)
    /* Some STM32 Platform */
    rcc_periph_clock_enable(RCC_RNG);
    rcc_periph_clock_enable(RCC_GPIOH);
    /* All of them use an external oscillator with bypass. */
    rcc_osc_off(RCC_HSE);
    rcc_osc_bypass_enable(RCC_HSE);
    rcc_osc_on(RCC_HSE);
    rcc_wait_for_osc_ready(RCC_HSE);
    # if defined(NUCLEO_BOARD)
    /* NUCLEO-STM32F2 Board */
    switch (clock) {
    case CLOCK_BENCHMARK:
        rcc_ahb_frequency = 30000000;
        rcc_apb1_frequency = 30000000;
        rcc_apb2_frequency = 30000000;
        _clock_freq = 30000000;
        rcc_set_hpre(RCC_CFGR_HPRE_DIV_4);
        rcc_set_ppre1(RCC_CFGR_PPRE_DIV_NONE);
        rcc_set_ppre2(RCC_CFGR_PPRE_DIV_NONE);
        rcc_osc_off(RCC_PLL);
        /* Configure the PLL oscillator (use CUBEMX tool). */
        rcc_set_main_pll_hse(8, 240, 2, 5);
        /* Enable PLL oscillator and wait for it to stabilize. */
        rcc_osc_on(RCC_PLL);
        rcc_wait_for_osc_ready(RCC_PLL);
        flash_dcache_enable();
        flash_icache_enable();
        flash_set_ws(FLASH_ACR_LATENCY_0WS);
        flash_prefetch_enable();
        break;
    case CLOCK_FAST:
    default:
        rcc_ahb_frequency = 120000000;
        rcc_apb1_frequency = 30000000;
        rcc_apb2_frequency = 60000000;
        _clock_freq = 120000000;
        rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
        rcc_set_ppre1(RCC_CFGR_PPRE_DIV_4);
        rcc_set_ppre2(RCC_CFGR_PPRE_DIV_2);
        rcc_osc_off(RCC_PLL);
        /* Configure the PLL oscillator (use CUBEMX tool). */
        rcc_set_main_pll_hse(8, 240, 2, 5);
        /* Enable PLL oscillator and wait for it to stabilize. */
        rcc_osc_on(RCC_PLL);
        rcc_wait_for_osc_ready(RCC_PLL);
        flash_dcache_enable();
        flash_icache_enable();
        flash_set_ws(FLASH_ACR_LATENCY_3WS);
        flash_prefetch_enable();
        break;
    }
    rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
    rcc_wait_for_sysclk_status(RCC_PLL);
    # else
# error Unsupported STM32F2 Board
    # endif
    #else
#error Unsupported platform
    #endif
}

void usart_setup() {
    #if defined(STM32F207ZG) || defined(STM32F7)
    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_USART3);
    #elif defined(DISCOVERY_BOARD)
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);
    #elif defined(NUCLEO_BOARD)
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);
    #else
#error Unsupported platform
    #endif

    #if defined(DISCOVERY_BOARD) || defined(NUCLEO_BOARD) || defined(STM32F7)
    gpio_set_output_options(SERIAL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ, SERIAL_PINS);
    gpio_set_af(SERIAL_GPIO, GPIO_AF7, SERIAL_PINS);
    gpio_mode_setup(SERIAL_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SERIAL_PINS);
    usart_set_baudrate(SERIAL_USART, SERIAL_BAUD);
    usart_set_databits(SERIAL_USART, 8);
    usart_set_stopbits(SERIAL_USART, USART_STOPBITS_1);
    usart_set_mode(SERIAL_USART, USART_MODE_TX_RX);
    usart_set_parity(SERIAL_USART, USART_PARITY_NONE);
    usart_set_flow_control(SERIAL_USART, USART_FLOWCONTROL_NONE);
    usart_disable_rx_interrupt(SERIAL_USART);
    usart_disable_tx_interrupt(SERIAL_USART);
    usart_enable(SERIAL_USART);
    #endif
}

void systick_setup() {
    /* Systick is always the same on libopencm3 */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_set_reload(0xFFFFFFu);
    systick_interrupt_enable();
    systick_counter_enable();
}
static volatile unsigned long long overflowcnt = 0;
void hal_setup(const enum clock_mode clock) {
    clock_setup(clock);
    usart_setup();
    systick_setup();
    rng_enable();

    // wait for the first systick overflow
    // improves reliability of the benchmarking scripts since it makes it much
    // less likely that the host will miss the start of the output
    unsigned long long old = overflowcnt;
    while (old == overflowcnt);
}

void hal_send_str(const char *in) {
    const char *cur = in;
    while (*cur) {
        usart_send_blocking(SERIAL_USART, *cur);
        cur += 1;
    }
    usart_send_blocking(SERIAL_USART, '\n');
}

void sys_tick_handler(void) {
    ++overflowcnt;
}

uint64_t hal_get_time() {
    while (true) {
        unsigned long long before = overflowcnt;
        unsigned long long result = (before + 1) * 16777216llu - systick_get_value();
        if (overflowcnt == before) {
            return result;
        }
    }
}

/* End of BSS is where the heap starts (defined in the linker script) */
extern char end;
static char *heap_end = &end;

void *__wrap__sbrk (int incr) {
    char *prev_heap_end;

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *) prev_heap_end;
}

size_t hal_get_stack_size(void) {
    register char *cur_stack;
    asm volatile ("mov %0, sp" : "=r" (cur_stack));
    return cur_stack - heap_end;
}

const uint32_t stackpattern = 0xDEADBEEFlu;

static void *last_sp = NULL;

void hal_spraystack(void) {

    char *_heap_end = heap_end;
    asm volatile ("mov %0, sp\n"
                  ".L%=:\n\t"
                  "str %2, [%1], #4\n\t"
                  "cmp %1, %0\n\t"
                  "blt .L%=\n\t"
                  : "+r" (last_sp), "+r" (_heap_end) : "r" (stackpattern) : "cc", "memory");
}

size_t hal_checkstack(void) {
    size_t result = 0;
    asm volatile("sub %0, %1, %2\n"
                 ".L%=:\n\t"
                 "ldr ip, [%2], #4\n\t"
                 "cmp ip, %3\n\t"
                 "ite eq\n\t"
                 "subeq %0, #4\n\t"
                 "bne .LE%=\n\t"
                 "cmp %2, %1\n\t"
                 "blt .L%=\n\t"
                 ".LE%=:\n"
                 : "+r"(result) : "r" (last_sp), "r" (heap_end), "r" (stackpattern) : "ip", "cc");
    return result;
}

/* Implement some system calls to shut up the linker warnings */

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
