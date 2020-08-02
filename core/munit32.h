#ifndef MUNIT32_H
#define MUNIT32_H
/* Extension unit for m_ext variants */
declare munit32 {
	input a[32];
	input b[32];
	/* see opcode.h for constant for fn */
	input fn[3];
	output q[32];
	func_out done();		/* Indicating division or multilication operation are done & value on q is valid in this cycle */
	func_out error();		/* Error occuried during operation (Zero division) */		
	/* The result of zero division on div, divu are -1 (maximum number) */
	/* The result of zero division on rem, remu are a (remainder) */
	func_in exe(a, b, fn) : q;
	/* reset internal state for canceling current operation */
	func_in reset();
}
#endif
