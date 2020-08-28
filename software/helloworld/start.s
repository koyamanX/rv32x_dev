.section .text.init
.globl _start
_start:
	j reset_vector
	.align 2
trap_vector:
	#if exception, interrupts occuries then exit simulator
	j _exit 
.extern main
.extern _stack_top
reset_vector:
	csrwi mstatus, 0
	la t0, trap_vector
	li t1, 0xfffffffc
	and t0, t0, t1
	csrw mtvec, t0
	la t0, _bss_start
	la t1, _bss_end
_clear_bss:
	sw zero, 0(t0)
	add t0, t0, 4
	bltu t0, t1, _clear_bss
	la sp, _stack_top
	add sp, sp, -16
	la t0, main
	csrw mepc, t0
	li t0, 0
	li t1, 0
	la ra, _exit
	mret
.globl _exit
_exit:
	li t0, 0
	li t1, 0xfff
_flush:
	blt t1, t0, __exit
	add t0, t0, 0x1
	j _flush
__exit:
	la t0, tohost
	sw a0, 0(t0)
	j _exit
	
.section .tohost
tohost: 
	.word 0x00000000
