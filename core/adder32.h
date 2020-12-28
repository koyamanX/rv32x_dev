#ifndef ADDER32_H
#define ADDER32_H
declare adder32 {
	input a[32];				/* augend */
	input b[32];				/* addend */
	output q[32];				/* sum */
	output cout;				/* carry-out */
	func_in exe(a, b) : q;		/* carry-out is also set */
}
#endif
