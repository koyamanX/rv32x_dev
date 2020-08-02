#ifndef ALU32_H
#define ALU32_H
declare alu32 {
	input a[32];	/* Operand A */
	input b[32];	/* Operand B */
	/* ALU select constant is defined in opcode.h as ALU_* variants */
	/* fn[3] must be zero in all case except ADD, SUB, SRA, SLL, SRL */
	/* 
	 ALU_ADD & fn[3] -> SUB 
	 ALU_ADD & !fn[3] -> ADD 
	 (ALU_SLL or ALU_SRL) & fn[3] -> Arithmetic shift 
	 (ALU_SLL or ALU_SRL) & !fn[3] -> Logical shift 
	*/
	input fn[4];	/* function select */
	output q[32];	/* result of operation */
	output z; 		/* result of operation is zero */
	func_in exe(a, b, fn) : q;	/* z is also set */
}
#endif
