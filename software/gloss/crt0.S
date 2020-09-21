#if 0
#include "newlib.h"
#endif

.section .text.init
.extern trap_handler
.extern machine_software_interrupt_handler
.extern machine_timer_interrupt_handler
.extern machine_external_interrupt_handler
_reset_vector:
	j _start
_trap_vector:
	j trap_handler							/* Exception handler */
	j trap_handler							/* Supervisor software interrupt */
	j trap_handler							/* vacant */
	j machine_software_interrupt_handler	/* Machine software interrupt */
	j trap_handler							/* User timer interrupt */
	j trap_handler							/* Supervisor timer interrupt */
	j trap_handler							/* vacant */
	j machine_timer_interrupt_handler		/* Machine timer interrupt */
	j trap_handler							/* User external interrupt */
	j trap_handler							/* Supervisor external interrupt */
	j trap_handler							/* vacant */
	j machine_external_interrupt_handler	/* Machine external interrupt */
.text
.globl _start
_start:
	csrwi mstatus, 0
	la t0, _trap_vector
	li t1, 0xfffffffc
	and t0, t1, t0
	ori t0, t0, 0x1
	csrw mtvec, t0
	la t0, _bss_start	# 4-byte aligned
	la t1, _bss_end		# 4-byte aligned
_clear_bss:
	sw zero, 0(t0)
	add t0, t0, 4
	bltu t0, t1, _clear_bss
_enter_u_mode:
	csrr t0, mstatus
	li t1, 0x00001800
	not t2, t1
	and t0, t0, t2
	or t0, t0, 0x8
	csrw mstatus, t0	# set mstatus.mpp, mstatus.mie
	la t0, _call_main
	csrw mepc, t0
	mret
.extern main
.extern exit
_call_main:
	li t0, 0
	li t1, 0
	li t2, 0
	la sp, _stack_top
	la ra, exit
	call main
	tail exit

