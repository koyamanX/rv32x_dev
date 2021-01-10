#ifndef SHIFTER32_H
#define SHIFTER32_H

#define SHIFTER_SL 1'b0
#define SHIFTER_SR 1'b1
#define SHIFTER_LOGICAL 1'b0
#define SHIFTER_ARITHMETIC 1'b1

declare shifter32 {
	input a[32];
	input b[5];
	input op;
	input arithmetic; /* 1 for arithmetic, 0 for logical */
	output q[32];
	func_in exe(a, b, op, arithmetic) : q;
}
#endif
