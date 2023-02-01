.text 
.globl _boot
_boot:
	# jump to entry point
	lui sp, 0x80020  
	lui ra, 0x80000
	ret
