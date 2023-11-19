.text 
.globl _boot
_boot:
	lui sp, 0x84000
	j main

