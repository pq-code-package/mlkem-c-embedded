/*SPDX-License-Identifier: MIT*/
MEMORY {
    irom   (x): org = 0x42000000, len = 0x400000
    drom   (r): org = 0x3C000000, len = 0x400000
    sram    (rw): org = 0x3FC80000, len = 0x50000
    rtc_ram (rx): org = 0x50000000, len = 0x2000
}

ENTRY(_start)

SECTIONS {
  .header (READONLY): AT(0)
  {
    _iflash_start = .;
    LONG(0xaedb041d)
    LONG(0xaedb041d)
  } > irom

  .text.entry : {
    KEEP(*(.text.entry))
  } > irom

  .text : {
    *(.text .stub .text.* .gnu.linkonce.t.*)
  } > irom

  .iram : {
    *(.iram .iram*)
  } > irom

  _iflash_end = .;
  _iflash_size = _iflash_end - _iflash_start;

  _dflash_start = ORIGIN(drom) + _iflash_size;
  .rodata _dflash_start : AT(_iflash_size) {
    *(.rodata .rodata* .srodata .srodata* .sdata2 .sdata2* .gnu.linkonce.r.*)
  } > drom

  _dflash_end = .;
  _dflash_size = _dflash_end - _dflash_start;

  .data ORIGIN(sram) : AT(_iflash_size + _dflash_size) {
    _data_start = .;
    *(.data .data* .gnu.linkonce.r.* .riscv.*)
  } > sram

  .sdata : {
    _sdata_start = .;
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } > sram
  _data_end = .;
  _data_lma_start = ORIGIN(drom) + LOADADDR(.data);

  .bss : {
    _bss_start = .;
    *(.bss .bss* .sbss .sbss* COMMON)
    _bss_end = .;
  } > sram

  __global_pointer$ = _sdata_start + 0x800;

  .stack : {
    _stack_bottom = .;
    _stack_top = ORIGIN(sram) + LENGTH(sram);
    _stack_size_min = 0x4000;
    ASSERT(_stack_bottom + _stack_size_min < _stack_top, "Error: no space for stack");
  }
}
