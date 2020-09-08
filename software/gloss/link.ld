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
  . = . + 0x02000000;
  . = ALIGN(0x4);
  _heap_end = .;
  . = ALIGN(0x10);
  . = 0x84008000;
  _stack_top = .;
}

