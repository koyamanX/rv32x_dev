.section .text.init
_reset:
	j _start

trap_vector:
.extern timer_handler
	/* if exception, interrupts occuries then exit simulator */
	j _exception_handler	/* User software interrupt || exception */
	j _exit					/* Supervisor software interrupt */
	j _exit					/* vacant */
	j _exit					/* Machine software interrupt */
	j _exit					/* User timer interrupt */
	j _exit					/* Supervisor timer interrupt */
	j _exit					/* vacant */
	j timer_handler		/* Machine timer interrupt */
	j _exit					/* User external interrupt */
	j _exit					/* Supervisor external interrupt */
	j _exit					/* vacant */
	j _exit					/* Machine external interrupt */

_exception_handler:
	j _exit					/* if exception occuries then exit */
.extern x
	sw t0, 0(sp)
	sw t1, 4(sp)
	la t0, x
	lw t1, 0(t0)
	add t1, t1, 1
	sw t1, 0(t0)
	lw t0, 0(sp)
	lw t1, 4(sp)
	mret	
.extern _stack_top
.extern main
.globl _start
_start:
	csrwi mstatus, 0		/* clear mstatus.mie */
	la t0, trap_vector
	li t1, 0xfffffffc
	and t0, t0, t1
	ori t0, t0, 0x1
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
	la ra, _exit
	li t0, 0
	li t1, 0
	mret
_exit:
	la t0, tohost
	sw a0, 0(t0)
	j _exit
	
.section .tohost
tohost: 
	.word 0x00000000
