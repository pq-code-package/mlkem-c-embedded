# SPDX-License-Identifier: MIT
    .section .text.entry
    .type _entry, @function
    .global _entry
_entry:
    .option push
    .option norelax
    la a0, _data_start
    la a1, _data_lma_start
    la a2, _data_end
    sub a2, a2, a0
    call memcpy
    .option pop
    j _start

    .section .text
    .type   _start, @function
    .global _start
_start:
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    la      a0, _bss_start
    la      a2, _bss_end
    sub     a2, a2, a0
    li      a1, 0
    call    memset

    la sp,  _stack_top

    /* Call main */
    la      a0, _main_argc
    lw      a0, 0(a0)
    la      a1, _main_argv
    li      a2, 0
    call    main
    tail    exit

    .size  _start, .-_start

    .section .data
    .global _main_argc
    .weak _main_argc
_main_argc:
    .word 1
_argv_0:
    .string ""
    .global _main_argv
    .weak _main_argv
_main_argv:
    .word _argv_0

