OUTPUT_ARCH( "riscv" )
ENTRY(main)
MEMORY {
	BRAM(rwx): ORIGIN = 0x80000000, LENGTH = 0x10000
}

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) } > BRAM
  .text : { *(.text) } > BRAM
  . = ALIGN(0x4);
  __data_start = .;
  .data : { *(.data) } > BRAM
  . = ALIGN(0x4);
  __data_end = .;
  __bss_start = .;
  .bss : { *(.bss) } > BRAM
  . = ALIGN(0x4);
  __bss_end = .;
  .zero : {
	  FILL(0x0); 
	  . = 0x80010000-__bss_end - 1;
	  BYTE(0x00);
  } > BRAM
}
