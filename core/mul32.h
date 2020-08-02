#ifndef MUL32_H
#define MUL32_H

declare mul32 {
	/* No pipeline */
	/* Takes 2 clocks to complete operation */
	input a[64];
	input b[64];
	output q[64];
	func_in reset();
	func_in exe(a, b);
	func_out done();
}
#endif
