.text 
.globl _boot
_boot:
	# jump to entry point
	la t0, 0x80000000
	jr t0

