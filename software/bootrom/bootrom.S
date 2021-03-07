.text 
.globl _boot
_boot:
	csrr a0, mhartid
	la a1, _dtb
	# jump to entry point
	la t0, 0x80000000
	jr t0
.align 2
.section .dtb
_dtb:
.incbin "rv32xsoc.dtb"
