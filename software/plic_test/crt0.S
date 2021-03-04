.section .text.init
_reset_vector:
	j _start
.extern trap_handler
_trap_vector:
	j trap_handler		# exception
	j _default_handler	# supervisor software interrupt
	j _default_handler	# reserved
	j _default_handler	# machine software interrupt
	j _default_handler	# reserved
	j _default_handler	# supervisor timer interrupt
	j _default_handler	# reserved
	j _default_handler	# machine timer interrupt
	j _default_handler	# reserved
	j _default_handler	# supervisor external interrupt
	j _default_handler	# reserved
	j _meip_handler		# machine external interrupt

.weak _default_handler
_default_handler:
	csrr t0, mcause
	csrr t1, mepc
	csrr t2, mtval
	j _default_handler

.weak _meip_handler
.extern meip_handler
_meip_handler:
	addi sp, sp, -0x80
	sw x1, 0x0(sp)
	sw x2, 0x4(sp)
	sw x3, 0x8(sp)
	sw x4, 0xc(sp)
	sw x5, 0x10(sp)
	sw x6, 0x14(sp)
	sw x7, 0x18(sp)
	sw x8, 0x1c(sp)
	sw x9, 0x20(sp)
	sw x10, 0x24(sp)
	sw x11, 0x28(sp)
	sw x12, 0x2c(sp)
	sw x13, 0x30(sp)
	sw x14, 0x34(sp)
	sw x15, 0x38(sp)
	sw x16, 0x3c(sp)
	sw x17, 0x40(sp)
	sw x18, 0x44(sp)
	sw x19, 0x48(sp)
	sw x20, 0x4c(sp)
	sw x21, 0x50(sp)
	sw x22, 0x54(sp)
	sw x23, 0x58(sp)
	sw x24, 0x5c(sp)
	sw x25, 0x60(sp)
	sw x26, 0x64(sp)
	sw x27, 0x68(sp)
	sw x28, 0x6c(sp)
	sw x29, 0x70(sp)
	sw x30, 0x74(sp)
	sw x31, 0x78(sp)
	call meip_handler	
	lw x1, 0x0(sp)
	lw x2, 0x4(sp)
	lw x3, 0x8(sp)
	lw x4, 0xc(sp)
	lw x5, 0x10(sp)
	lw x6, 0x14(sp)
	lw x7, 0x18(sp)
	lw x8, 0x1c(sp)
	lw x9, 0x20(sp)
	lw x12, 0x2c(sp)
	lw x13, 0x30(sp)
	lw x14, 0x34(sp)
	lw x15, 0x38(sp)
	lw x16, 0x3c(sp)
	lw x17, 0x40(sp)
	lw x18, 0x44(sp)
	lw x19, 0x48(sp)
	lw x20, 0x4c(sp)
	lw x21, 0x50(sp)
	lw x22, 0x54(sp)
	lw x23, 0x58(sp)
	lw x24, 0x5c(sp)
	lw x25, 0x60(sp)
	lw x26, 0x64(sp)
	lw x27, 0x68(sp)
	lw x28, 0x6c(sp)
	lw x29, 0x70(sp)
	lw x30, 0x74(sp)
	lw x31, 0x78(sp)
	addi sp, sp, 0x80
	mret
	
.text
_start:
	csrwi mstatus, 0	# clear mstatus.mie, mstatus.sie
	la t0, _trap_vector
	ori t0, t0, 0x1
	csrw mtvec, t0
# clear bss section
	la t0, _bss_start	# 4-byte aligned
	la t1, _bss_end		# 4-byte aligned
_clear_bss:
	sw zero, 0(t0)
	add t0, t0, 4
	bltu t0, t1, _clear_bss
.extern main
.extern exit
_call_main:
	li t0, 0x800
	csrw mie, t0
	li t0, 0
	li t1, 0
	li t2, 0
	la sp, _stack_top
	la ra, exit
	li a0, 1		# argc
	la a1, _argv	# argv
	li a2, 0		# env
	csrwi mstatus, 0x8	# set mstatus.mie
	call main
	call exit

.align 4
_arg0:
	.string "main"	
.align 4
_argv:
	.word _arg0
