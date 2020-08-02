#ifndef INST_DEC_H
#define INST_DEC_H
declare inst_dec {
	input inst[32];
	output rd[5];				/* destination register */
	output rs1[5];				/* source register 1 */
	output rs2[5];				/* source register 2 */
	output funct3[3];			/* funct 3 */   
	output funct7[7];			/* funct 7 */
	output funct12[12];			/* funct 12 */
	output beq;					/* is BEQ? */
	output bne;					/* is BNE? */
	output blt;					/* is BLT? */
	output bltu;				/* is BLTU? */
	output bge;					/* is BGE? */
	output bgeu;				/* is BGEU?*/
	output imm[32];				/* immdeiate */
	output alu_fn[4];			/* alu function selelct */
	output wb;					/* if writeback to GPR needed ? */

	func_in exe(inst);
	/* types of operation */
	func_out load();
	func_out store();
	func_out branch();
	func_out jalr();
	func_out misc_mem();
	func_out jal();
	func_out op_imm();
	func_out system();
	func_out lui();
	func_out auipc();
	func_out op();
	func_out illegal_instruction();
	func_out m_ext();
}
#endif
