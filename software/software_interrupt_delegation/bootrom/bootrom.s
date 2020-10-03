.text 
.globl _boot
_boot:
	# jump to entry point
	lui ra, 0x80000
	ret

