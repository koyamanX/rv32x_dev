#ifndef IMM_GEN_H
#define IMM_GEN_H

#define IMM_ADDI4SPN 4'b0001
#define IMM_LW_SW 4'b0010
#define IMM_JAL 4'b0011
#define IMM_ADDI16SP 4'b0100
#define IMM_LUI 4'b0101
#define IMM_BEQZ_BNEZ 4'b0110
#define IMM_LWSP 4'b0111
#define IMM_SWSP 4'b1000
#define IMM_S_COMMON 4'b1001
#define IMM_U_COMMON 4'b1010

declare imm_gen {
	input inst[32];				/* instruction to extract imm */
	output imm[32];				/* extracted imm */
	input code[4];
	/* types of instruction */
	func_in i_type(inst) : imm;
	func_in s_type(inst) : imm;
	func_in b_type(inst) : imm;
	func_in u_type(inst) : imm;
	func_in j_type(inst) : imm;
	func_in c_type(inst, code) : imm;
}
#endif
