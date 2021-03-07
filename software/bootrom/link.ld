OUTPUT_ARCH( "riscv" )
ENTRY(_boot)
MEMORY {
	ROM(rx): ORIGIN = 0x00000000, LENGTH = 4K
}

SECTIONS
{
  . = 0x00000000;
  .text.init : { *(.text.init) } > ROM
  .text : { *(.text) } > ROM
  . = ALIGN(0x4);
  __data_start = .;
  .data : { *(.data) } > ROM
  . = ALIGN(0x4);
  __data_end = .;
  __bss_start = .;
  .bss : { *(.bss) } > ROM
  . = ALIGN(0x4);
  __bss_end = .;
  .dtb : { *(.dtb) } > ROM
}
