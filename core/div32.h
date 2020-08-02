#ifndef DIV32_H
#define DIV32_H
declare div32 {
	/* No pipeline & takes 34 clocks to complete operation */
	/* On zero division, error signal is asserted & result (q) is 
		returned in the same cycle with the maximum number of uint32 
		and remainder (r) is original value in input port a 
		no further operation is taken place */
	/* sign signal is used to indicate operands are signed */
	input a[32];
	input b[32];
	output q[32];
	output r[32];
	func_in exe(a,b);
	func_in sign();
	func_out done();
	func_out error();
	func_in reset();
}
#endif
