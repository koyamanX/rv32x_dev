.globl trap_handler
.weak trap_handler
trap_handler:
_save_all_register:
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
_check_cause:
	csrr t0, mcause
	li t1, 11
	beq t0, t1, _syscall_enter
	li t1, 8
	beq t0, t1, _syscall_enter
_restore_return_value:
	lw x10, 0x24(sp)
	lw x11, 0x28(sp)
_restore_all_register_except_return_value_and_exit:
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
_trap_ret:
	mret
_advance_mepc:
	csrr t0, mepc
	add t0, t0, 0x4
	csrw mepc, t0
	ret
_syscall_enter:
	call _advance_mepc
.extern syscall
	call syscall	# C-function
	j _restore_all_register_except_return_value_and_exit
