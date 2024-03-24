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
	func_out beq();					/* is BEQ? */
	func_out bne();					/* is BNE? */
	func_out blt();					/* is BLT? */
	func_out bltu();				/* is BLTU? */
	func_out bge();					/* is BGE? */
	func_out bgeu();				/* is BGEU?*/
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
	func_out misc_mem_fence();
	func_out misc_mem_fencei();
	func_out jal();
	func_out op_imm();
	func_out system();
	func_out system_csrrw();
	func_out system_csrrc();
	func_out system_csrrs();
	func_out system_ecall();
	func_out system_ebreak();
	func_out system_mret();
	func_out system_sret();
	func_out system_uret();
	func_out system_wfi();
	func_out system_sfence_vma();
	func_out uimm();
	func_out lui();
	func_out auipc();
	func_out op();
	func_out illegal_instruction();
	func_out m_ext();
	func_out amo();
}
#endif
