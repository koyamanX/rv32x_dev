#ifndef SUB32_H
#define SUB32_H
declare sub32 {
	input a[33];				/* augend */
	input b[33];				/* addend */
	output q[32];				/* sum */
	output cout;				/* borrow */
	func_in exe(a, b) : q;	/* carry-out is also set */
}
#endif
