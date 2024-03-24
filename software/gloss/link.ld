OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  .text : { *(.text) }
  . = ALIGN(0x4);
  .rodata : { *(.rodata) }
  .sdata : { *(.sdata) }
  .data : { *(.data) }
  . = ALIGN(0x4);
  _bss_start = .;
  .bss : { *(.bss) }
  .sbss : { *(.sbss) }
  . = ALIGN(0x4);
  _bss_end = .;
  _heap_start = .;
  . = . + 0x00006000;
  . = ALIGN(0x4);
  _heap_end = .;
  . = 0x80008000;
  . = ALIGN(0x10);
  _stack_top = .;
}

